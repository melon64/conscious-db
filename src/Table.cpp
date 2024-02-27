#include "Table.h"

Table::Table() : num_rows(0), pager(nullptr) {}

void Table::db_open(const std::string& filename) {
    pager = std::make_unique<Pager>(filename);
    num_rows = ((pager->get_file_length() / PAGE_SIZE) * ROWS_PER_PAGE) + ((pager->get_file_length() % PAGE_SIZE) / sizeof(Row));
}

void Table::db_close() {
    size_t num_full_pages = num_rows / ROWS_PER_PAGE;
    for (size_t i = 0; i < num_full_pages; i++) {
        if (!pager->get_pages()[i]) {
            continue;
        }
        pager->flush(i, PAGE_SIZE);
        pager->get_pages()[i].reset();
    }

    size_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        size_t page_num = num_full_pages;
        if (pager->get_pages()[page_num]) {
            pager->flush(page_num, num_additional_rows * sizeof(Row));
            pager->get_pages()[page_num].reset();
        }
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
    return Cursor(this, 0);
}

Cursor Table::end() {
    return Cursor(this, num_rows);
}

int Table::size() {
    return num_rows;
}

void* Table::row_slot(size_t row_num) {
    size_t page_num = row_num / ROWS_PER_PAGE;
    char* page = pager->get_page(page_num);
    size_t row_offset = row_num % ROWS_PER_PAGE;
    size_t byte_offset = row_offset * sizeof(Row);
    return page + byte_offset;
}
