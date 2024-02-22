#ifndef CURSOR_H
#define CURSOR_H

#include "Table.h"
#include "Row.h"

class Cursor {
public:
    Cursor(Table* table, size_t row) : table(table), row_num(row), end_of_table(row >= table->size()) {}
    
    Row operator*() {
        size_t page_num = row_num / ROWS_PER_PAGE;
        void *page = table->pager->get_page(page_num);
        size_t row_offset = row_num % ROWS_PER_PAGE;
        size_t byte_offset = row_offset * sizeof(Row);
        Row row;
        row.deserialize(static_cast<char*>(page + byte_offset));
        return row;
    }

    Cursor& operator++() {
        row_num++;
        if (row_num >= table->size()) {
            end_of_table = true;
        }
        return *this;
    }

    Cursor& operator--() {
        row_num--;
        if (row_num < table->size()) {
            end_of_table = false;
        }
        return *this;
    }

    bool operator==(const Cursor& other) const {
        return table == other.table && row_num == other.row_num;
    }

private:
    Table* table;
    size_t row_num;
    bool end_of_table;

};

#endif