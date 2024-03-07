#include "Table.h"

Table::Table() : root_page_num(0), pager(nullptr) {}

void Table::db_open(const std::string& filename) {
    pager = std::make_shared<Pager>(filename);
    if (pager->size() == 0){
        void *root_node = pager->get_page(0);
        BTreeNode node = BTreeNode(root_node);
        node.initialize_leaf_node();
        node.set_root(true);
    }
}

void Table::db_close() {
    for (size_t i = 0; i < pager->get_pages().size(); i++) {
        if (!pager->get_pages()[i]) {
            continue;
        }
        pager->flush(i);
        pager->get_pages()[i].reset();
    }

    if (pager->close()) {
        std::cerr << "Error closing db file\n";
        exit(EXIT_FAILURE);
    }
    pager.reset();
}

int Table::insert(const Row& row) {
    void* n = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(n);
    uint32_t num_cells = *(node.leaf_node_num_cells());
    uint32_t key = row.get_id();
    Cursor cursor = table_find(key);

    if (cursor.cell_num < num_cells) {
        uint32_t key_at_index = *(node.leaf_node_key(cursor.cell_num));
        if (key_at_index == key) {
            return -1;
        }
    }

    leaf_node_insert(cursor, row.get_id(), row);

    return 1;
}

void Table::leaf_node_insert(Cursor cursor, const uint32_t &key, const Row &value) {
    void *n = pager->get_page(cursor.page_num);
    BTreeNode node = BTreeNode(n);

    size_t num_cells = *(node.leaf_node_num_cells());
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor.cell_num < num_cells){
        for (uint32_t i = num_cells; i > cursor.cell_num; i--){
            std::copy(
                static_cast<char*>(node.leaf_node_cell(i - 1)),
                static_cast<char*>(node.leaf_node_cell(i - 1)) + LEAF_NODE_CELL_SIZE,
                static_cast<char*>(node.leaf_node_cell(i))
            );
        }
    }

    *(node.leaf_node_num_cells()) += 1;
    *(node.leaf_node_key(cursor.cell_num)) = key;
    value.serialize(static_cast<char*>(node.leaf_node_value(cursor.cell_num)));
}

void Table::leaf_node_split_and_insert(Cursor cursor, const uint32_t &key, const Row &value) {
    /*
    new node and move half the cells
    insert new cell in one of the nodes
    update parent or create new parent
    */
    void *old_node = pager->get_page(cursor.page_num);
    BTreeNode old_node_b = BTreeNode(old_node);
    uint32_t old_node_max = old_node_b.get_max_key();
    uint32_t new_page_num = pager->get_unused_page_num();

    void *new_node = pager->get_page(new_page_num);
    BTreeNode new_node_b = BTreeNode(new_node);
    new_node_b.initialize_leaf_node();
    // *(new_node_b.node_parent()) = *(old_node_b.node_parent());

    *(new_node_b.leaf_node_next_leaf()) = *(old_node_b.leaf_node_next_leaf());
    *(old_node_b.leaf_node_next_leaf()) = new_page_num;

    /*
    divide existing cells between old (left) and new (right) node
    starting from the rightmost cell, move each cell to correct position
    */    

    for (uint32_t i = LEAF_NODE_MAX_CELLS; i > 0; i--){
        void *destination_node;
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT){
            destination_node = new_node;
        }
        else {
            destination_node = old_node;
        }
        BTreeNode destination_node_b = BTreeNode(destination_node);
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void *destination = destination_node_b.leaf_node_cell(index_within_node);

        if (i == cursor.cell_num){
            value.serialize(static_cast<char*>(destination_node_b.leaf_node_value(index_within_node)));
            *(destination_node_b.leaf_node_key(index_within_node)) = key;
        }
        else if (i > cursor.cell_num){
            std::copy(
                static_cast<char*>(old_node_b.leaf_node_cell(i - 1)),
                static_cast<char*>(old_node_b.leaf_node_cell(i - 1)) + LEAF_NODE_CELL_SIZE,
                static_cast<char*>(destination)
            );

        }
        else {
            std::copy(
                static_cast<char*>(old_node_b.leaf_node_cell(i)),
                static_cast<char*>(old_node_b.leaf_node_cell(i + 1)),
                static_cast<char*>(destination)
            );
        }
    }  
    //update cell count
    *(old_node_b.leaf_node_num_cells()) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(new_node_b.leaf_node_num_cells()) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if (old_node_b.is_root()){
        return create_new_root(new_page_num);
    } 
    else{
        std::cerr << "Need to implement updating parent after split\n";
        exit(EXIT_FAILURE);
    }
}

void Table::create_new_root(uint32_t right_child_page_num) {
    /*
    old root is copied to new page, becomes left child
    new root node is created with right child
    new root node points to two children
    */

    uint32_t left_child_page_num = pager->get_unused_page_num();
    void *root_node = pager->get_page(root_page_num);
    BTreeNode root_node_b = BTreeNode(root_node);
    void *left_child = pager->get_page(left_child_page_num);
    BTreeNode left_child_b = BTreeNode(left_child);

    //old root copied to left child
    std::copy(
        static_cast<char*>(root_node),
        static_cast<char*>(root_node + PAGE_SIZE),
        static_cast<char*>(left_child)
    );
    left_child_b.set_root(false);

    //root node is a new internal node with one key and two children
    root_node_b.initialize_internal_node();
    root_node_b.set_root(true);
    *(root_node_b.internal_node_num_keys()) = 1;
    *(root_node_b.internal_node_child(0)) = left_child_page_num;
    uint32_t left_child_max_key = left_child_b.get_max_key();
    *(root_node_b.internal_node_key(0)) = left_child_max_key;
    *(root_node_b.internal_node_right_child()) = right_child_page_num;
}

Cursor Table::table_find(uint32_t key) {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);

    if(node.get_node_type() == NodeType::Leaf){
        return leaf_node_find(key, root_page_num);
    }
    else {
        return internal_node_find(key, root_page_num);
    }
}

Cursor Table::internal_node_find(uint32_t key, uint32_t page_num) {
    void *root_node = pager->get_page(page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_keys = *(node.internal_node_num_keys());

    uint32_t min_index = 0;
    uint32_t max_index = num_keys;

    while (min_index != max_index){
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_to_right = *(node.internal_node_key(index));
        if (key_to_right >= key){
            max_index = index;
        }
        else {
            min_index = index + 1;
        }
    }
    
    uint32_t child_num = *(node.internal_node_child(min_index));
    void *child = pager->get_page(child_num);
    BTreeNode child_node = BTreeNode(child);

    switch (child_node.get_node_type()){
        case NodeType::Leaf:
            return leaf_node_find(key, child_num);
        case NodeType::Internal:
            return internal_node_find(key, child_num);
    }
}

Cursor Table::leaf_node_find(uint32_t key, uint32_t page_num) {
    void *root_node = pager->get_page(page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());

    Cursor cursor(this, page_num, 0, num_cells == 0);

    uint32_t min_index = 0;
    uint32_t max_index = num_cells;

    //binary search
    while (max_index != min_index){
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_at_index = *(node.leaf_node_key(index));

        if (key == key_at_index){
            cursor.cell_num = index;
            return cursor;
        }
        if (key < key_at_index){
            max_index = index;
        }
        else {
            min_index = index + 1;
        }
    }

    cursor.cell_num = min_index;
    return cursor;
}

void Table::select() {
    for (Cursor cursor = this->start(); !cursor.end_of_table; ++cursor) {
        Row row;
        int key = *(static_cast<uint32_t*>(*cursor));
        void *temp = static_cast<char*>(*cursor) + LEAF_NODE_KEY_SIZE;
        row.deserialize(static_cast<char*>(temp));
        std::cout << "index: " << key << " value: " << row.get_id() << " " << row.get_username() << " " << row.get_email() << std::endl;
    }
}

Cursor Table::start() {
    Cursor cursor = table_find(0);
    void *root_node = pager->get_page(cursor.page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());
    return Cursor(this, cursor.page_num, 0, num_cells == 0);
}

Cursor Table::end() { //deprecated
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());

    return Cursor(this, root_page_num, num_cells, true);
}

int Table::size() {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    //if root is leaf, return number of cells
    if (node.get_node_type() == NodeType::Leaf){
        return *(node.leaf_node_num_cells());
    }

    return 0;

}

void Table::print_constants(){
        std::cout << "ROW_SIZE: " << sizeof(Row) << std::endl;
        std::cout << "COMMON_NODE_HEADER_SIZE: " << (int) COMMON_NODE_HEADER_SIZE << std::endl;
        std::cout << "LEAF_NODE_HEADER_SIZE: " << LEAF_NODE_HEADER_SIZE << std::endl;
        std::cout << "LEAF_NODE_CELL_SIZE: " << LEAF_NODE_CELL_SIZE << std::endl;
        std::cout << "LEAF_NODE_SPACE_FOR_CELLS: " << LEAF_NODE_SPACE_FOR_CELLS << std::endl;
        std::cout << "LEAF_NODE_MAX_CELLS: " << LEAF_NODE_MAX_CELLS << std::endl;
}

void Table::print_tree(uint32_t page_num, uint32_t indentation_level){
    void *root_node = pager->get_page(page_num);
    BTreeNode node = BTreeNode(root_node);
    uint32_t num_keys, child;

    switch(node.get_node_type()){
        case NodeType::Leaf:
            indent(indentation_level);
            std::cout << "leaf (size " << *(node.leaf_node_num_cells()) << ")\n";
            for (uint32_t i = 0; i < *(node.leaf_node_num_cells()); i++){
                indent(indentation_level + 1);
                std::cout << " - " << *(node.leaf_node_key(i)) << "\n";
            }
            break;
        case NodeType::Internal:
            indent(indentation_level);
            std::cout << "internal (size " << *(node.internal_node_num_keys()) << ")\n";
            for (uint32_t i = 0; i < *(node.internal_node_num_keys()); i++){
                child = *(node.internal_node_child(i));
                print_tree(child, indentation_level + 1);

                indent(indentation_level + 1);
                std::cout << " - key " << *(node.internal_node_key(i)) << "\n";
            }
            child = *(node.internal_node_right_child());
            print_tree(child, indentation_level + 1);
            break;
    }
}

void Table::indent(uint32_t level){
    for (uint32_t i = 0; i < level; i++){
        std::cout << "  ";
    }
}

