#include <iostream>
#include <string>
#include <vector>

#include "InputBuffer.h"
#include "MetaCommand.h"
#include "Statement.h"
#include "Table.h"
#include "Row.h"

using namespace std;

// build cmds
// cmake .. -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"
// make all

int main(int argc, char* argv[]) {
    if (argc < 2){
        cout << "Must supply a database filename.\n";
        exit(EXIT_FAILURE);
    }

    string filename = argv[1];
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    Table *table = new Table();
    table->db_open(filename);

    while (true) {
        input_buffer->print_prompt();
        input_buffer->read_input(cin);
        
        string input = input_buffer->get_input();

        if (input[0] == '.') {
            MetaCommand meta_command;
            switch (meta_command.execute_meta_command(input, table)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    cout << "Unrecognized command '" << input << "'.\n";
                    continue;
            }
        }
        
        Statement statement;

        switch (statement.prepare_statement(input)) {
            case PREPARE_SUCCESS:
                break;
            case (PREPARE_NEGATIVE_ID):
                cout << "ID must be positive.\n";
                continue;
            case (PREPARE_STRING_TOO_LONG):
                cout << "String is too long.\n";
                continue;
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
}
