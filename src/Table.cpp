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

void Table::internal_node_insert(uint32_t parent_page_num, uint32_t child_page_num){
    std::cout << "\ninternal node insert\n";
    void *parent = pager->get_page(parent_page_num);
    void *child = pager->get_page(child_page_num);

    BTreeNode parent_b = BTreeNode(parent);
    BTreeNode child_b = BTreeNode(child);

    uint32_t child_max = get_node_max_key(child_b);
    uint32_t index = parent_b.internal_node_find_child(child_max);

    uint32_t original_num_keys = *(parent_b.internal_node_num_keys());

    if (original_num_keys >= INTERNAL_NODE_MAX_CELLS){
        std::cout << "internal node split since max keys reached\n";
        std::cout << "original num keys: " << original_num_keys << "\n";
        internal_node_split_and_insert(parent_page_num, child_page_num);
        return;
    }

    uint32_t right_child_page_num = *(parent_b.internal_node_right_child());

    //An internal node with a right child of INVALID_PAGE_NUM is empty
    if (right_child_page_num == INVALID_PAGE_NUM){
        std::cout << "right child is invalid\n";
        *(parent_b.internal_node_right_child()) = child_page_num;
        return;
    }

    void *right_child = pager->get_page(right_child_page_num);
    BTreeNode right_child_b = BTreeNode(right_child);

    /*
    If we are already at the max number of cells for a node, we cannot increment
    before splitting.
    */

   *(parent_b.internal_node_num_keys()) = original_num_keys + 1;

    if (child_max > get_node_max_key(right_child_b)){
        //replace right child with child
        std::cout << "child max is greater than right child max\n";
        *(parent_b.internal_node_child(original_num_keys)) = right_child_page_num;
        *(parent_b.internal_node_key(original_num_keys)) = get_node_max_key(right_child_b);
        *(parent_b.internal_node_right_child()) = child_page_num;   
    }
    else{
        for (uint32_t i = original_num_keys; i > index; i--) {
            std::cout << "copying index " << i << " to " << i + 1 << "\n";
            std::copy(
                (parent_b.internal_node_cell(i - 1)),
                (parent_b.internal_node_cell(i - 1)) + INTERNAL_NODE_CELL_SIZE,
                (parent_b.internal_node_cell(i))  
            );
        }
        *(parent_b.internal_node_child(index)) = child_page_num;
        *(parent_b.internal_node_key(index)) = child_max;
    }

    //print number of keys
    std::cout << "num keys after insert: " << *(parent_b.internal_node_num_keys()) << "\n";
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
    *(new_node_b.node_parent()) = *(old_node_b.node_parent());

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
        uint32_t parent_page_num = *(old_node_b.node_parent());
        uint32_t new_max = new_node_b.get_max_key();
        void *parent = pager->get_page(parent_page_num);
        BTreeNode parent_b = BTreeNode(parent);

        parent_b.update_internal_node_key(old_node_max, new_max);

        internal_node_insert(parent_page_num, new_page_num);
    }
}

void Table::internal_node_split_and_insert(uint32_t parent_page_num, uint32_t child_page_num){
    std::cout << "\ninternal node split and insert\n\n";
    uint32_t old_page_num = parent_page_num;
    void *old_node = pager->get_page(old_page_num);
    BTreeNode old_node_b = BTreeNode(old_node);
    uint32_t old_max = get_node_max_key(old_node_b);

    void *child = pager->get_page(child_page_num);
    BTreeNode child_b = BTreeNode(child);
    uint32_t child_max = get_node_max_key(child_b);

    uint32_t new_page_num = pager->get_unused_page_num();

    bool splitting_root = old_node_b.is_root();
    void *parent, *new_node;

    /*
    Declaring a flag before updating pointers which
    records whether this operation involves splitting the root -
    if it does, we will insert our newly created node during
    the step where the table's new root is created. If it does
    not, we have to insert the newly created node into its parent
    after the old node's keys have been transferred over
    */
    if (splitting_root){
        std::cout << "splitting root\n";
        create_new_root(new_page_num);
        parent = pager->get_page(root_page_num);
        BTreeNode parent_b = BTreeNode(parent);
        
        old_page_num = *(parent_b.internal_node_child(0));
        old_node = pager->get_page(old_page_num);
    }
    else {
        std::cout << "not splitting root\n";
        parent = pager->get_page(*old_node_b.node_parent());
        new_node = pager->get_page(new_page_num);
        BTreeNode new_node_b = BTreeNode(new_node);
        new_node_b.initialize_internal_node();
    }

    uint32_t *old_node_num_keys = old_node_b.internal_node_num_keys();
    
    uint32_t cur_page_num = *old_node_b.internal_node_right_child();
    void *cur = pager->get_page(cur_page_num);
    BTreeNode cur_b = BTreeNode(cur);

    //First put right child into new node and set right child of old node to invalid page number
    internal_node_insert(new_page_num, cur_page_num);
    *(cur_b.node_parent()) = new_page_num;
    *(old_node_b.internal_node_right_child()) = INVALID_PAGE_NUM;

    std::cout<<"old_node_num_keys before middle key: "<<*old_node_num_keys<<std::endl;

    //For each key until you get to the middle key, move the key and the child to the new node
    for (int i = *old_node_num_keys - 1; i > *old_node_num_keys / 2; i--){
        // if (i >= *old_node_num_keys) {
        //     std::cout << "Tried to access child at index " << i << " when it doesn't exist\n";
        //     continue;
        // }
        std::cout << "num keys: " << *old_node_num_keys << " index " << i << "\n";
        cur_page_num = *old_node_b.internal_node_child(i);
        cur = pager->get_page(cur_page_num);
        cur_b = BTreeNode(cur);

        internal_node_insert(new_page_num, cur_page_num);
        *(cur_b.node_parent()) = new_page_num;
        *old_node_num_keys -= 1;
    }


    //Set child before middle key, which is now the highest key, to be node's right child,and decrement number of keys
    *(old_node_b.internal_node_right_child()) = *old_node_b.internal_node_child(*old_node_num_keys-1);
    *old_node_num_keys -= 1;

    //Determine which of the two nodes after the split should contain the child to be inserted, and insert the child
    uint32_t max_after_split = get_node_max_key(old_node_b);
    uint32_t destination_page_num = (child_max < max_after_split) ? old_page_num : new_page_num;
    BTreeNode parent_b = BTreeNode(parent);

    parent_b.update_internal_node_key(old_max, get_node_max_key(old_node_b));
    if (!splitting_root){
        internal_node_insert(*old_node_b.node_parent(), new_page_num);
        BTreeNode new_node_b = BTreeNode(new_node);
        *(new_node_b.node_parent()) = *old_node_b.node_parent();
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
    void *right_child = pager->get_page(right_child_page_num);
    BTreeNode right_child_b = BTreeNode(right_child);
    void *left_child = pager->get_page(left_child_page_num);
    BTreeNode left_child_b = BTreeNode(left_child);

    if (root_node_b.get_node_type() == NodeType::Internal){
        left_child_b.initialize_internal_node();
        right_child_b.initialize_internal_node();
    }

    //old root copied to left child
    std::copy(
        static_cast<char*>(root_node),
        static_cast<char*>(root_node + PAGE_SIZE),
        static_cast<char*>(left_child)  
    );
    left_child_b.set_root(false);

    if (left_child_b.get_node_type() == NodeType::Internal){
        void *child;
        for (uint32_t i = 0; i < *(left_child_b.internal_node_num_keys()); i++){
            child = pager->get_page(*(left_child_b.internal_node_child(i)));
            BTreeNode child_b = BTreeNode(child);
            *(child_b.node_parent()) = left_child_page_num;
        }
    }

    //root node is a new internal node with one key and two children
    root_node_b.initialize_internal_node();
    root_node_b.set_root(true);
    *(root_node_b.internal_node_num_keys()) = 1;
    *(root_node_b.internal_node_child(0)) = left_child_page_num;

    uint32_t left_child_max_key = get_node_max_key(left_child_b);

    *(root_node_b.internal_node_key(0)) = left_child_max_key;
    *(root_node_b.internal_node_right_child()) = right_child_page_num;
    *(left_child_b.node_parent()) = root_page_num;
    *(right_child_b.node_parent()) = root_page_num;
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
    void *n = pager->get_page(page_num);
    BTreeNode node = BTreeNode(n);
    
    uint32_t child_index = node.internal_node_find_child(key);
    uint32_t child_num = *(node.internal_node_child(child_index));
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

uint32_t Table::get_node_max_key(BTreeNode node){
    switch (node.get_node_type()){
        case NodeType::Internal:
        {
            uint32_t page = *node.internal_node_right_child();
            void *right_child = pager->get_page(page);
            BTreeNode right_child_b = BTreeNode(right_child);
            return get_node_max_key(right_child_b);
        }
        case NodeType::Leaf:
            return *node.leaf_node_key(*node.leaf_node_num_cells() - 1);
    }
    return -1;
}

int Table::size() {
    Cursor cursor = this->start();
    int count = 0;
    while (!cursor.end_of_table) {
        count++;
        ++cursor;
    }
    return count;
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
            if (*(node.internal_node_num_keys()) > 0){
                for (uint32_t i = 0; i < *(node.internal_node_num_keys()); i++){
                    child = *(node.internal_node_child(i));
                    print_tree(child, indentation_level + 1);

                    indent(indentation_level + 1);
                    std::cout << " - key " << *(node.internal_node_key(i)) << "\n";
                }
                child = *(node.internal_node_right_child());
                print_tree(child, indentation_level + 1);
            }
            break;
    }
}

void Table::indent(uint32_t level){
    for (uint32_t i = 0; i < level; i++){
        std::cout << "  ";
    }
}

