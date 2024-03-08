#ifndef PAGER_H
#define PAGER_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "Row.h"

static const size_t PAGE_SIZE = 4096;
static const size_t TABLE_MAX_PAGES = 100;
static const size_t INVALID_PAGE_NUM = UINT32_MAX;


class Pager {
public:
    Pager(const std::string& filename);

    char* get_page(size_t page_num);

    void flush(size_t page_num);

    int close();

    size_t get_file_length() const;

    size_t get_unused_page_num();

    size_t size() const;

    std::vector<std::unique_ptr<char[]>>& get_pages();

private:
    std::streamoff file_length;
    std::fstream file_stream;
    size_t num_pages;
    std::vector<std::unique_ptr<char[]>> pages;
};

#endif
