#include "Cursor.h"
#include "Table.h"

Cursor::Cursor(Table* table, size_t row) : table(table), row_num(row), end_of_table(row >= table->size()) {}

void* Cursor::operator*() {
    size_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pager->get_page(page_num);
    size_t row_offset = row_num % ROWS_PER_PAGE;
    size_t byte_offset = row_offset * sizeof(Row);  
    return page + byte_offset;
}

Cursor& Cursor::operator++() {
    row_num++;
    if (row_num >= table->size()) {
        end_of_table = true;
    }
    return *this;
}

Cursor& Cursor::operator--() {
    row_num--;
    if (row_num < table->size()) {
        end_of_table = false;
    }
    return *this;
}

bool Cursor::operator!=(const Cursor& other) const {
    return (table != other.table || row_num != other.row_num || end_of_table != other.end_of_table);
}
