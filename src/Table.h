#ifndef TABLE_H
#define TABLE_H

#include "Row.h"
#include "Pager.h"
#include "Cursor.h"
#include "BTreeNode.h"

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

    void leaf_node_insert(Cursor *cursor, uint32_t key, Row* value);

    void select();

    Cursor start();

    Cursor end();

    int size();

private:
    size_t root_page_num;
    std::shared_ptr<Pager> pager;

    // void* row_slot(size_t row_num);
};

#endif
