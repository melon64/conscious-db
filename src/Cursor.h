#ifndef CURSOR_H
#define CURSOR_H

#include "Table.h"

class Table;
class Cursor {
public:
    Cursor(Table* table, size_t page_num, size_t cell_num, bool is_end_of_table = false);

    void* operator*();

    Cursor& operator++();

    bool operator!=(const Cursor& other) const;

// private:
    Table* table;
    size_t page_num;
    size_t cell_num;
    bool end_of_table;
};

#endif
