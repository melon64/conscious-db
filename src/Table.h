#ifndef TABLE_H
#define TABLE_H

#include "Row.h"
#include "Table.h"

#include <iostream>
#include <cstring>
#include <vector>

static const size_t PAGE_SIZE = 4096;
static const size_t TABLE_MAX_PAGES = 100;
static const size_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
static const size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Table {
public:
    Table() : num_rows(0) {
        pages.resize(TABLE_MAX_PAGES, nullptr);
    }

    ~Table() {
        for (auto page : pages) {
            if (page != nullptr)    {
                delete[] static_cast<char*>(page);
            }
        }
    }

    bool insert(const Row& row) {
        if (num_rows >= TABLE_MAX_ROWS) {
            return false;
        }
        void* destination = row_slot(num_rows);
        if (destination) {
            row.serialize(static_cast<char*>(destination));
            num_rows++;
            return true;
        }
        return false;
    }

    void select() {
        for (size_t i = 0; i < num_rows; i++) {
            Row row;
            row.deserialize(static_cast<const char*>(row_slot(i)));
            std::cout << row.get_id() << " " << row.get_username() << " " << row.get_email() << std::endl;
        }
    }

    int size() {
        return num_rows;
    }

private:
    size_t num_rows;
    std::vector<void*> pages;

    void* row_slot(size_t row_num) {
        size_t page_num = row_num / ROWS_PER_PAGE;
        if (!pages[page_num]) {
            pages[page_num] = new char[PAGE_SIZE];
        }
        size_t row_offset = row_num % ROWS_PER_PAGE;
        size_t byte_offset = row_offset * sizeof(Row);
        return static_cast<char*>(pages[page_num]) + byte_offset;
    }
};

#endif
