#ifndef CURSOR_H
#define CURSOR_H

#include "Table.h"

class Table;
class Cursor {
public:
    Cursor(Table* table, size_t row);

    void* operator*();

    Cursor& operator++();

    Cursor& operator--();

    bool operator!=(const Cursor& other) const;

private:
    Table* table;
    size_t row_num;
    bool end_of_table;
};

#endif
