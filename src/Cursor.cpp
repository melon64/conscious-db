#include "Cursor.h"
#include "Table.h"
#include "BTreeNode.h"

Cursor::Cursor(Table* table, size_t page_num, size_t cell_num, bool is_end_of_table) : table(table), page_num(page_num), cell_num(cell_num), end_of_table(is_end_of_table) {}

void* Cursor::operator*() {
    BTreeNode node = BTreeNode(table->pager->get_page(page_num));
    return node.leaf_node_cell(cell_num);
}

Cursor& Cursor::operator++() {
    BTreeNode node = BTreeNode(table->pager->get_page(page_num));
    cell_num++;
    if (cell_num >= *(node.leaf_node_num_cells())){ //maybe change to >?
        end_of_table = true;
    }
    return *this;
}

bool Cursor::operator!=(const Cursor& other) const {
    return page_num != other.page_num || cell_num != other.cell_num || end_of_table != other.end_of_table || table != other.table;
}

