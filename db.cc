#include <iostream>
#include <string>
#include <vector>

#include "InputBuffer.h"
#include "MetaCommand.h"
#include "Statement.h"

using namespace std;

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    while (true) {
        input_buffer->print_prompt();
        input_buffer->read_input();
        
        string input = input_buffer->get_input();

        if (input[0] == '.') {
            MetaCommand meta_command;
            switch (meta_command.execute_meta_command(input)) {
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
            case PREPARE_UNRECOGNIZED_STATEMENT:
                cout << "Unrecognized keyword at start of '" << input << "'.\n";
                continue;
        }

        statement.execute_statement();
        cout << "Executed.\n";
    }
}
