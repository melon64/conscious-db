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


