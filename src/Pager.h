#ifndef PAGER_H
#define PAGER_H

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>

#include "Table.h"

using namespace std;

class Pager {
public:
    Pager() : file_descriptor_(-1), file_length_(0) {
        for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
            pages_[i] = nullptr;
        }
    }

    ~Pager() {
        for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
            if (pages_[i] != nullptr) {
                delete[] static_cast<char*>(pages_[i]);
            }
        }
    }

    void pager_open(string filename){
        ifstream file;
        file.open(filename, ios::binary);
        if (file.is_open()) {
            file_descriptor_ = file.rdbuf()->pubseekoff(0, ios::cur, ios::in);
            file.seekg(0, ios::end);
            file_length_ = file.tellg();
            file.seekg(0, ios::beg);
            file.close();
        } else {
            cout << "Could not open file\n";
            exit(EXIT_FAILURE);
        }
    }

private:
    int file_descriptor_;
    size_t file_length_;
    void* pages_[TABLE_MAX_PAGES];
};

#endif