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

TEST(DBTest, MemoryTest){
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
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();
    
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    string username = string(32, 'a');
    string email = string(255, 'a');
    string sIn = "insert 0 " + username + " " + email;
    stringstream ss;
    ss << sIn;   

    input_buffer->read_input(ss);
    string input = input_buffer->get_input();
    
    Statement statement;
    switch (statement.prepare_statement(input)) {
        case PrepareResult::SUCCESS:
            break;
        case PrepareResult::NEGATIVE_ID:
            cout << "ID must be positive.\n";
            break;
        case PrepareResult::STRING_TOO_LONG:
            cout << "String is too long.\n";
            break;
        case PrepareResult::SYNTAX_ERROR:
            cout << "Syntax error. Could not parse statement.\n";
            break;
        case PrepareResult::UNRECOGNIZED_STATEMENT:
            cout << "Unrecognized keyword at start of '" << input << "'.\n";
            break;
    }

    switch (statement.execute_statement(table)) {
        case ExecuteResult::SUCCESS:
            cout << "Executed.\n";
            break;
        case ExecuteResult::TABLE_FULL:
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

// TEST(DBTest, StressTest1) {
//     string filename = "test.db";

//     ofstream file(filename, ios::binary);
//     file.close();

//     InputBuffer* input_buffer = InputBuffer::GetInstance();
//     Table *table = new Table();
//     table->db_open(filename);

//     for (int i = 0; i < 1000; i++) {
//         string sIn = "insert " + to_string(i) + " user user" + to_string(i) + "@gmail.com";
//         stringstream ss;
//         ss << sIn;

//         input_buffer->read_input(ss);
//         string input = input_buffer->get_input();
        
//         Statement statement;
//         statement.prepare_statement(input);
//         statement.execute_statement(table);
//     }

//     string sIn1 = "select";
//     stringstream ss1;
//     ss1 << sIn1;
//     input_buffer->read_input(ss1);
//     string input1 = input_buffer->get_input();

//     Statement statement1;
//     statement1.prepare_statement(input1);
//     statement1.execute_statement(table);
    
//     ASSERT_EQ(table->size(), 1000);
// }

// TEST(DBTest, PersistenceTest){
//     string filename = "test.db";

//     ofstream file(filename, ios::binary);
//     file.close();
    
//     InputBuffer* input_buffer = InputBuffer::GetInstance();
//     Table *table = new Table();
//     table->db_open(filename);

//     for (int i = 0; i < 100; i++) {
//         string username = "test";
//         string email = "test@test.test";
//         string sIn = "insert " + to_string(i) + " " + username + " " + email;
//         stringstream ss;
//         ss << sIn;  

//         input_buffer->read_input(ss);
//         string input = input_buffer->get_input();
        
//         Statement statement;
//         switch (statement.prepare_statement(input)) {
//             case PrepareResult::SUCCESS:
//                 break;
//             case PrepareResult::NEGATIVE_ID:
//                 cout << "ID must be positive.\n";
//                 continue;
//             case PrepareResult::STRING_TOO_LONG:
//                 cout << "String is too long.\n";
//                 continue;
//             case PrepareResult::SYNTAX_ERROR:
//                 cout << "Syntax error. Could not parse statement.\n";
//                 continue;
//             case PrepareResult::UNRECOGNIZED_STATEMENT:
//                 cout << "Unrecognized keyword at start of '" << input << "'.\n";
//                 continue;
//         }

//         switch (statement.execute_statement(table)) {
//             case ExecuteResult::SUCCESS:
//                 cout << "Executed.\n";
//                 break;
//             case ExecuteResult::TABLE_FULL:
//                 cout << "Error: Table full.\n";
//                 continue;
//         }
//     }

//     table->db_close();

//     Table *table1 = new Table();
//     table1->db_open(filename);

//     for (int i = 100; i < 200; i++) {
//         string username = string(32, 'a');
//         string email = string(255, 'a');
//         string sIn = "insert " + to_string(i) + " " + username + " " + email;
//         stringstream ss;
//         ss << sIn;  

//         input_buffer->read_input(ss);
//         string input = input_buffer->get_input();
        
//         Statement statement;
//         switch (statement.prepare_statement(input)) {
//             case PrepareResult::SUCCESS:
//                 break;
//             case PrepareResult::NEGATIVE_ID:
//                 cout << "ID must be positive.\n";
//                 continue;
//             case PrepareResult::STRING_TOO_LONG:
//                 cout << "String is too long.\n";
//                 continue;
//             case PrepareResult::SYNTAX_ERROR:
//                 cout << "Syntax error. Could not parse statement.\n";
//                 continue;
//             case PrepareResult::UNRECOGNIZED_STATEMENT:
//                 cout << "Unrecognized keyword at start of '" << input << "'.\n";
//                 continue;
//         }

//         switch (statement.execute_statement(table1)) {
//             case ExecuteResult::SUCCESS:
//                 cout << "Executed.\n";
//                 break;
//             case ExecuteResult::TABLE_FULL:
//                 cout << "Error: Table full.\n";
//                 continue;
//         }
//     }

//     table1->db_close();
//     delete table1;

//     Table *table2 = new Table();
//     table2->db_open(filename);

//     for (int i = 200; i < 300; i++) {
//         string username = "test";
//         string email = "test@test.test";
//         string sIn = "insert " + to_string(i) + " " + username + " " + email;
//         stringstream ss;
//         ss << sIn;  

//         input_buffer->read_input(ss);
//         string input = input_buffer->get_input();
        
//         Statement statement;
//         switch (statement.prepare_statement(input)) {
//             case PrepareResult::SUCCESS:
//                 break;
//             case PrepareResult::NEGATIVE_ID:
//                 cout << "ID must be positive.\n";
//                 continue;
//             case PrepareResult::STRING_TOO_LONG:
//                 cout << "String is too long.\n";
//                 continue;
//             case PrepareResult::SYNTAX_ERROR:
//                 cout << "Syntax error. Could not parse statement.\n";
//                 continue;
//             case PrepareResult::UNRECOGNIZED_STATEMENT:
//                 cout << "Unrecognized keyword at start of '" << input << "'.\n";
//                 continue;
//         }

//         switch (statement.execute_statement(table2)) {
//             case ExecuteResult::SUCCESS:
//                 cout << "Executed.\n";
//                 break;
//             case ExecuteResult::TABLE_FULL:
//                 cout << "Error: Table full.\n";
//                 continue;
//         }
//     }

//     table2->db_close();
//     delete table2;

//     Table *table3 = new Table();
//     table3->db_open(filename);

//     string sIn1 = "select";
//     stringstream ss1;
//     ss1 << sIn1;
//     input_buffer->read_input(ss1);
//     string input1 = input_buffer->get_input();

//     Statement statement1;
//     statement1.prepare_statement(input1);
//     statement1.execute_statement(table3);

//     ASSERT_EQ(table3->size(), 300);
// }

TEST(DBTestBTree, SplitTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();
    
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    for (int i = 0; i < 14; i++) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table->print_tree(0, 0);

    ASSERT_EQ(table->size(), 14);
}

TEST(DBTestBTree, PersistenceTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();
    
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    for (int i = 0; i < 5; i++) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table->print_tree(0, 0);

    table->db_close();

    Table *table1 = new Table();
    table1->db_open(filename);

    for (int i = 5; i < 10; i++) {
        string username = string(32, 'a');
        string email = string(255, 'a');
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table1)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table1->print_tree(0, 0);

    table1->db_close();
    delete table1;

    Table *table2 = new Table();
    table2->db_open(filename);

    for (int i = 10; i < 15; i++) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table2)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table2->db_close();
    delete table2;

    Table *table3 = new Table();
    table3->db_open(filename);

    table3->print_tree(0, 0);

    string sIn1 = "select";
    stringstream ss1;
    ss1 << sIn1;
    input_buffer->read_input(ss1);
    string input1 = input_buffer->get_input();

    Statement statement1;
    statement1.prepare_statement(input1);
    statement1.execute_statement(table3);

    ASSERT_EQ(table3->size(), 15);
}

TEST(DBTestBTree, TreeTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();

    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    for (int i = 0; i < 13; i++) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table->print_tree(0, 0);

    ASSERT_EQ(table->size(), 13);

}
TEST(DBTestBTree, ConstantsTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();

    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    table->print_constants();

    // Assert ROW_SIZE
    ASSERT_EQ(sizeof(Row), 304);

    // Assert COMMON_NODE_HEADER_SIZE
    ASSERT_EQ(COMMON_NODE_HEADER_SIZE, 6);

    // Assert LEAF_NODE_HEADER_SIZE
    ASSERT_EQ(LEAF_NODE_HEADER_SIZE, 10);

    // Assert LEAF_NODE_CELL_SIZE
    ASSERT_EQ(LEAF_NODE_CELL_SIZE, 308);

    // Assert LEAF_NODE_SPACE_FOR_CELLS
    ASSERT_EQ(LEAF_NODE_SPACE_FOR_CELLS, 4086);

    // Assert LEAF_NODE_MAX_CELLS
    ASSERT_EQ(LEAF_NODE_MAX_CELLS, 13);

    table->db_close();
}

TEST(DBTestBTree, OrderTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();

    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    for (int i = 3; i > 0; i--) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table->print_tree(0, 0);

    ASSERT_EQ(table->size(), 3);

}

TEST(DBTestBTree, StressTest){
    string filename = "test.db";

    ofstream file(filename, ios::binary);
    file.close();

    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    for (int i = 0; i < 30; i++) {
        string username = "test";
        string email = "test@test.test";
        string sIn = "insert " + to_string(i) + " " + username + " " + email;
        stringstream ss;
        ss << sIn;  

        input_buffer->read_input(ss);
        string input = input_buffer->get_input();
        
        Statement statement;
        switch (statement.prepare_statement(input)) {
            case PrepareResult::SUCCESS:
                break;
            case PrepareResult::NEGATIVE_ID:
                cout << "ID must be positive.\n";
                continue;
            case PrepareResult::STRING_TOO_LONG:
                cout << "String is too long.\n";
                continue;
            case PrepareResult::SYNTAX_ERROR:
                cout << "Syntax error. Could not parse statement.\n";
                continue;
            case PrepareResult::UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        switch (statement.execute_statement(table)) {
            case ExecuteResult::SUCCESS:
                cout << "Executed.\n";
                break;
            case ExecuteResult::TABLE_FULL:
                cout << "Error: Table full.\n";
                continue;
        }
    }

    table->print_tree(0, 0);

    ASSERT_EQ(table->size(), 30);
}
