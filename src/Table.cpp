#include "Table.h"

Table::Table() : root_page_num(0), pager(nullptr) {}

void Table::db_open(const std::string& filename) {
    pager = std::make_shared<Pager>(filename);
    if (pager->size() == 0){
        void *root_node = pager->get_page(0);
        BTreeNode node = BTreeNode(root_node);
        node.initialize_leaf_node();
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

    if ((*node.leaf_node_num_cells()) >= LEAF_NODE_MAX_CELLS) {
        return 0;
    }

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
        std::cerr << "Need to implement splitting a leaf node\n";
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

Cursor Table::table_find(uint32_t key) {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);

    if(node.get_node_type() == NodeType::Leaf){
        return leaf_node_find(key);
    }
    else {
        std::cerr << "Need to implement searching an internal node\n";
        exit(EXIT_FAILURE);
    }
}

Cursor Table::leaf_node_find(uint32_t key) {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());

    Cursor cursor(this, root_page_num, 0, num_cells == 0);

    uint32_t min_index = 0;
    uint32_t max_index = num_cells;

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
    for (Cursor cursor = this->start(); cursor != this->end(); ++cursor) {
        Row row;
        int key = *(static_cast<uint32_t*>(*cursor));
        void *temp = static_cast<char*>(*cursor) + LEAF_NODE_KEY_SIZE;
        row.deserialize(static_cast<char*>(temp));
        std::cout << "index: " << key << " value: " << row.get_id() << " " << row.get_username() << " " << row.get_email() << std::endl;
    }
}

Cursor Table::start() {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());
    return Cursor(this, root_page_num, 0, num_cells == 0);
}

Cursor Table::end() {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    size_t num_cells = *(node.leaf_node_num_cells());

    return Cursor(this, root_page_num, num_cells, true);
}

int Table::size() {
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    return *(node.leaf_node_num_cells());
}

void Table::print_constants(){
        std::cout << "ROW_SIZE: " << sizeof(Row) << std::endl;
        std::cout << "COMMON_NODE_HEADER_SIZE: " << (int) COMMON_NODE_HEADER_SIZE << std::endl;
        std::cout << "LEAF_NODE_HEADER_SIZE: " << LEAF_NODE_HEADER_SIZE << std::endl;
        std::cout << "LEAF_NODE_CELL_SIZE: " << LEAF_NODE_CELL_SIZE << std::endl;
        std::cout << "LEAF_NODE_SPACE_FOR_CELLS: " << LEAF_NODE_SPACE_FOR_CELLS << std::endl;
        std::cout << "LEAF_NODE_MAX_CELLS: " << LEAF_NODE_MAX_CELLS << std::endl;
}

void Table::print_tree(){
    void *root_node = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(root_node);
    node.print_leaf_node();
}


