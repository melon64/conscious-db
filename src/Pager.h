#ifndef PAGER_H
#define PAGER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "Row.h"

static const size_t PAGE_SIZE = 4096;
static const size_t TABLE_MAX_PAGES = 100;
static const size_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
static const size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Pager {
public:
    Pager(const std::string& filename);

    char* get_page(size_t page_num);

    void flush(size_t page_num, size_t size);

    int close();

    size_t get_file_length() const;

    std::vector<std::unique_ptr<char[]>>& get_pages();

private:
    std::streamoff file_length;
    std::fstream file_stream;
    std::vector<std::unique_ptr<char[]>> pages;
};

#endif
