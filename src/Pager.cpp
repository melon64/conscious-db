#include "Pager.h"

#include <iostream>

Pager::Pager(const std::string& filename) : file_length(0), pages() {
    file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (!file_stream.is_open()) {
        std::cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }
    file_length = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    pages.resize(TABLE_MAX_PAGES);
}

char* Pager::get_page(size_t page_num) {
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
                // std::cerr << "Reached EOF\n";
                file_stream.clear();
            }
            else if (file_stream.fail()) {
                std::cerr << "Error reading file p1\n";
                exit(EXIT_FAILURE);
            }
        }
        if (page_num >= num_pages){
            num_pages = page_num + 1;
        }
    }
    
    return pages[page_num].get();
}

void Pager::flush(size_t page_num) {
    if (!pages[page_num]) {
        std::cerr << "Tried to flush null page\n";
        exit(EXIT_FAILURE);
    }

    file_stream.seekp(page_num * PAGE_SIZE);
    if (file_stream.fail()) {
        std::cerr << "Error seeking to page for writing\n";
        exit(EXIT_FAILURE);
    }

    file_stream.write(pages[page_num].get(), PAGE_SIZE);
    file_stream.flush();

    if (file_stream.fail()) {
        std::cerr << "Error writing to file\n";
        exit(EXIT_FAILURE);
    }

    char* buffer = new char[PAGE_SIZE];

    file_stream.seekg(page_num * PAGE_SIZE);
    if (file_stream.fail()) {
        std::cerr << "Error seeking file p2\n";
        exit(EXIT_FAILURE);
    }

    file_stream.read(buffer, PAGE_SIZE);
    if (file_stream.fail()) {
        std::cerr << "Error reading file p2\n";
        exit(EXIT_FAILURE);
    }
}

int Pager::close() {
    file_stream.close();
    if (file_stream.fail()) {
        return 1;
    }
    return 0;
}

size_t Pager::get_file_length() const {
    return file_length;
}

std::vector<std::unique_ptr<char[]>>& Pager::get_pages() {
    return pages;
}
