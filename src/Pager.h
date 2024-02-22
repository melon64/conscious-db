#ifndef PAGER_H
#define PAGER_H

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

using namespace std;

static const size_t PAGE_SIZE = 4096;
static const size_t TABLE_MAX_PAGES = 100;
static const size_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
static const size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

class Pager {
public:
    Pager(const std::string& filename) : file_length(0), pages() {
        file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
        if (!file_stream.is_open()) {
            std::cerr << "Unable to open file\n";
            exit(EXIT_FAILURE);
        }
        file_length = file_stream.tellg();
        file_stream.seekg(0, std::ios::beg); 

        pages.resize(TABLE_MAX_PAGES);
    }

    char *get_page(size_t page_num) {
        if (page_num > TABLE_MAX_PAGES) {
            std::cerr << "Tried to fetch page number out of bounds\n";
            exit(EXIT_FAILURE);
        }

        if (!pages[page_num]) {
            pages[page_num] = std::make_unique<char[]>(PAGE_SIZE);
            size_t num_pages = file_length / PAGE_SIZE;
            if (file_length % PAGE_SIZE) {
                num_pages++;
            }
            if (page_num < num_pages) {
                file_stream.seekg(page_num * PAGE_SIZE);
                if (file_stream.fail()) {
                    std::cerr << "Error seeking to page for reading p1\n";
                    exit(EXIT_FAILURE);
                }
                file_stream.read(pages[page_num].get(), PAGE_SIZE);
                if (file_stream.eof()) {
                    file_stream.clear();
                }
                else if (file_stream.fail()) {
                    std::cerr << "Error reading file p1\n";
                    exit(EXIT_FAILURE);
                }
            }
        }
        return pages[page_num].get();
    }

    void flush(size_t page_num, size_t size){
        if (!pages[page_num]) {
            std::cerr << "Tried to flush null page\n";
            exit(EXIT_FAILURE);
        }

        file_stream.seekp(page_num * PAGE_SIZE);
        if (file_stream.fail()) {   
            std::cerr << "Error seeking to page for writing\n";
            exit(EXIT_FAILURE);
        }

        file_stream.write(pages[page_num].get(), size);
        file_stream.flush();

        if (file_stream.fail()) {
            std::cerr << "Error writing to file\n";
            exit(EXIT_FAILURE);
        }

        char *buffer= new char[PAGE_SIZE];

        file_stream.seekg(page_num * PAGE_SIZE);
        if (file_stream.fail()) {
            std::cerr << "Error seeking file p2\n";
            exit(EXIT_FAILURE);
        }

        file_stream.read(buffer, size);
        if (file_stream.fail()) {
            std::cerr << "Error reading file p2\n";
            exit(EXIT_FAILURE);
        }
    }

    int close () {
        file_stream.close();
        if (file_stream.fail()) {
            return 1;
        }
        return 0;
    }

    size_t get_file_length() const {
        return file_length;
    }

    vector<unique_ptr<char[]>> &get_pages() {
        return pages;
    }

private:
    streamoff file_length;
    std::fstream file_stream;
    std::vector<std::unique_ptr<char[]>> pages;
};

#endif