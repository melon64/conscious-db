#ifndef TABLE_H
#define TABLE_H

#include "Row.h"
#include <iostream>
#include <cstring>
#include <vector>

class Table {
public:
    static const uint32_t PAGE_SIZE = 4096;
    static const uint32_t TABLE_MAX_PAGES = 100;
    static const uint32_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
    static const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

    Table() : num_rows(0) {
        pages.resize(TABLE_MAX_PAGES, nullptr);
    }

    ~Table() {
    for (auto page : pages) {
        if (page != nullptr) {
            delete[] static_cast<char*>(page);
        }
    }
}

    bool insert(const Row& row) {
        if (num_rows >= TABLE_MAX_ROWS) {
            return false;
        }

        void* destination = row_slot(num_rows);
        row.serialize(static_cast<char*>(destination));
        num_rows++;
        return true;
    }

    void select() {
        for (uint32_t i = 0; i < num_rows; i++) {
            Row row;
            row.deserialize(static_cast<const char*>(row_slot(i)));
            std::cout << row.get_id() << " " << row.get_username() << " " << row.get_email() << std::endl;
        }
    }

    int size() {
        return num_rows;
    }

private:
    uint32_t num_rows;
    std::vector<void*> pages;

    void* row_slot(uint32_t row_num) {
        uint32_t page_num = row_num / ROWS_PER_PAGE;
        if (!pages[page_num]) {
            pages[page_num] = new char[PAGE_SIZE];
        }
        uint32_t row_offset = row_num % ROWS_PER_PAGE;
        uint32_t byte_offset = row_offset * sizeof(Row);
        return static_cast<char*>(pages[page_num]) + byte_offset;
    }
};

#endif
