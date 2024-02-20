#ifndef TABLE_H
#define TABLE_H

#include "Row.h"
#include "Table.h"
#include "Pager.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <memory>

class Table {
public:
     Table() : num_rows(0), pager(nullptr) {}    

    void db_open(const std::string& filename) {
        pager = std::make_unique<Pager>(filename);
        num_rows = pager->get_file_length() / sizeof(Row);
        cout << "rows per page: " << ROWS_PER_PAGE << "\n";
        cout << "num_rows: " << num_rows << endl;
    }

    void db_close(){
        size_t num_full_pages = num_rows / ROWS_PER_PAGE;
        cout << "num_full_pages: " << num_full_pages << endl;
        for (size_t i = 0; i < num_full_pages; i++) {
            if (!pager->get_pages()[i]) {
                continue;
            }
            pager->flush(i, PAGE_SIZE);
            pager->get_pages()[i].reset();
        }

        cout << "num_rows full flushed: " << num_rows << endl;

        size_t num_additional_rows = num_rows % ROWS_PER_PAGE;
        cout << "num_additional_rows: " << num_additional_rows << endl;
        if (num_additional_rows > 0) {
            size_t page_num = num_full_pages;
            if (pager->get_pages()[page_num]) {
                pager->flush(page_num, num_additional_rows * sizeof(Row));
                pager->get_pages()[page_num].reset();
            }
        }

        cout << "num_additional_rows partial flushed: " << num_additional_rows << endl;

        if (pager->close()) {
            std::cerr << "Error closing db file\n";
            exit(EXIT_FAILURE);
        }
        pager.reset();
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
    std::unique_ptr<Pager> pager;

    void* row_slot(size_t row_num) {
        size_t page_num = row_num / ROWS_PER_PAGE;
        char* page = pager->get_page(page_num);
        size_t row_offset = row_num % ROWS_PER_PAGE;
        size_t byte_offset = row_offset * sizeof(Row);
        return page + byte_offset;
    }
};

#endif
