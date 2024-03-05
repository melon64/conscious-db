#include "Table.h"

Table::Table() : root_page_num(0), pager(nullptr) {}

void Table::db_open(const std::string& filename) {
    pager = std::make_shared<Pager>(filename);
    if (pager->get_pages().size() == 0){
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

bool Table::insert(const Row& row) {
    void* n = pager->get_page(root_page_num);
    BTreeNode node = BTreeNode(n);
    if ((*node.leaf_node_num_cells()) >= LEAF_NODE_MAX_CELLS) {
        return false;
    }

    Row *new_row = new Row(row);
    Cursor *cursor =  new Cursor(end());

    leaf_node_insert(cursor, new_row->get_id(), new_row);

    delete cursor;
    return true;
}

void Table::leaf_node_insert(Cursor *cursor, uint32_t key, Row* value) {
    void *n = pager->get_page(cursor->page_num);
    BTreeNode node = BTreeNode(n);

    size_t num_cells = *(node.leaf_node_num_cells());
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // split
        return;
    }

    if (cursor->cell_num < num_cells){
        for (uint32_t i = num_cells; i > cursor->cell_num; i--){
            std::copy(
                static_cast<char*>(node.leaf_node_cell(i - 1)),
                static_cast<char*>(node.leaf_node_cell(i - 1)) + LEAF_NODE_CELL_SIZE,
                static_cast<char*>(node.leaf_node_cell(i))
            );
        }
    }

    *(node.leaf_node_num_cells()) += 1;
    *(node.leaf_node_key(cursor->cell_num)) = key;
    value->serialize(static_cast<char*>(node.leaf_node_value(cursor->cell_num)));
}

void Table::select() {
    for (Cursor cursor = this->start(); cursor != this->end(); ++cursor) {
        Row row;
        row.deserialize(static_cast<char*>(*cursor));
        std::cout << row.get_id() << " " << row.get_username() << " " << row.get_email() << std::endl;
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


