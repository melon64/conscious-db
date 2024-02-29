#include "Table.h"

Table::Table() : num_rows(0), pager(nullptr) {}

void Table::db_open(const std::string& filename) {
    pager = std::make_shared<Pager>(filename);
    num_rows = ((pager->get_file_length() / PAGE_SIZE) * ROWS_PER_PAGE) + ((pager->get_file_length() % PAGE_SIZE) / sizeof(Row));
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
    if (num_rows >= TABLE_MAX_ROWS) {
        return false;
    }
    Cursor end = this->end();
    row.serialize(static_cast<char*>(*end));
    num_rows++;
    return true;
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
    size_t num_cells = *leaf_node_num_cells(root_node);
    return Cursor(this, root_page_num, 0, num_cells == 0);
}

Cursor Table::end() {
    void *root_node = pager->get_page(root_page_num);
    size_t num_cells = *leaf_node_num_cells(root_node);

    return Cursor(this, root_page_num, num_cells, true);
}

int Table::size() {
    return num_rows;
}

// void* Table::row_slot(size_t row_num) {
//     size_t page_num = row_num / ROWS_PER_PAGE;
//     char* page = pager->get_page(page_num);
//     size_t row_offset = row_num % ROWS_PER_PAGE;
//     size_t byte_offset = row_offset * sizeof(Row);
//     return page + byte_offset;
// }
