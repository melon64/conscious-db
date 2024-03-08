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

    int insert(const Row& row);

    void leaf_node_insert(Cursor cursor, const uint32_t &key, const Row &value);

    void internal_node_insert(uint32_t parent_page_num, uint32_t child_page_num);

    void leaf_node_split_and_insert(Cursor cursor, const uint32_t &key, const Row &value);

    void internal_node_split_and_insert(uint32_t old_page_num, uint32_t new_page_num);

    void select();

    Cursor start();

    Cursor end(); //deprecated

    int size();

    void print_constants();

    void print_tree(uint32_t page_num, uint32_t indentation_level);

    Cursor table_find(uint32_t key);

    Cursor leaf_node_find(uint32_t key, uint32_t page_num);

    Cursor internal_node_find(uint32_t key, uint32_t page_num);

    uint32_t get_node_max_key(BTreeNode node);

    void create_new_root(uint32_t right_child_page_num);

private:
    size_t root_page_num;
    std::shared_ptr<Pager> pager;

    void indent(uint32_t level);
};

#endif
