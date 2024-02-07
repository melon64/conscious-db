#ifndef META_COMMAND_H
#define META_COMMAND_H

#include <iostream>
#include <string>

using namespace std;

typedef enum { META_COMMAND_SUCCESS, META_COMMAND_UNRECOGNIZED_COMMAND } MetaCommandResult;

class MetaCommand{
    public:
        MetaCommandResult execute_meta_command(string input) {
            if (input == ".exit") {
                exit(EXIT_SUCCESS);
            } else {
                return META_COMMAND_UNRECOGNIZED_COMMAND;
            }
        }
};

#endif
