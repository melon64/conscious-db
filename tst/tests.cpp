#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "InputBuffer.h"
#include "MetaCommand.h"
#include "Statement.h"
#include "Table.h"
#include "Row.h"

using namespace std;


TEST(REPLTest, ReadInput) {
    stringstream ss;
    ss << "test";
    InputBuffer *input_buffer = InputBuffer::GetInstance();
    input_buffer->read_input(ss);
    ASSERT_EQ(input_buffer->get_input(), "test"); //input "test" for cin
}

TEST(DBTEST, MemoryTest){
    Row row;
    row.set_id(1);
    row.set_username("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    row.set_email("test");

    std::vector<void*> pages;
    size_t PAGE_SIZE = 4096;
    size_t TABLE_MAX_PAGES = 100;
    size_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
    size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

    pages.resize(TABLE_MAX_PAGES, nullptr);

    pages[0] = new char[PAGE_SIZE];
    void* destination = static_cast<char*>(pages[0]);
    row.serialize(static_cast<char*>(destination));

    Row row1;
    row1.deserialize(static_cast<const char*>(destination));
    std::cout << row1.get_id() << " " << row1.get_username() << " " << row1.get_email() << std::endl;

    ASSERT_EQ(std::string(row1.get_username()), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

TEST(DBTest, MemoryTest1){
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();

    string username = string(32, 'a');
    string email = string(255, 'a');
    string sIn = "insert 0 " + username + " " + email;
    stringstream ss;
    ss << sIn;   

    input_buffer->read_input(ss);
    string input = input_buffer->get_input();
    
    Statement statement;
    cout << "prepare statement" << endl;
    switch (statement.prepare_statement(input)) {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_SYNTAX_ERROR:
            cout << "Syntax error. Could not parse statement.\n";
            break;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            cout << "Unrecognized keyword at start of '" << input << "'.\n";
            break;
    }

    cout << "execute statement" << endl;
    switch (statement.execute_statement(table)) {
        case EXECUTE_SUCCESS:
            cout << "Executed.\n";
            break;
        case EXECUTE_TABLE_FULL:
            cout << "Error: Table full.\n";
            break;
    }


    string sIn1 = "select";
    stringstream ss1;
    ss1 << sIn1;
    input_buffer->read_input(ss1);
    string input1 = input_buffer->get_input();

    Statement statement1;
    statement1.prepare_statement(input1);
    statement1.execute_statement(table);
    
    ASSERT_EQ(table->size(), 1);
}

TEST(DBTest, StressTest1) {
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();

    for (int i = 0; i < 1000; i++) {
        string sIn = "insert " + to_string(i) + " user user" + to_string(i) + "@gmail.com";
        stringstream ss;
        ss << sIn;

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case EXECUTE_SUCCESS:
                cout << "Executed.\n";
                break;
            case EXECUTE_TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    string sIn1 = "select";
    stringstream ss1;
    ss1 << sIn1;
    input_buffer->read_input(ss1);
    string input1 = input_buffer->get_input();

    Statement statement1;
    statement1.prepare_statement(input1);
    statement1.execute_statement(table);
    
    ASSERT_EQ(table->size(), 1000);
}
