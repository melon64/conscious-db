#ifndef TABLE_H
#define TABLE_H

#include "Row.h"
#include "Pager.h"
#include "Cursor.h"

#include <iostream>
#include <memory>
#include <string>

class Cursor;
class Table {
public:
    friend class Cursor;
    Table();

    void db_open(const std::string& filename);

    void db_close();

    bool insert(const Row& row);

    void select();

    Cursor start();

    Cursor end();

    int size();

private:
    size_t num_rows;
    std::unique_ptr<Pager> pager;

    void* row_slot(size_t row_num);
};

#endif
