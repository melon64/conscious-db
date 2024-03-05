#include "MetaCommand.h"

MetaCommandResult MetaCommand::execute_meta_command(std::string input, Table* table) {
    if (input == ".exit") {
        table->db_close();
        std::exit(EXIT_SUCCESS);
    } 
    else if (input == ".constants"){
        std::cout << "Constants:\n";
        table->print_constants();
    }
    else if (input == ".btree"){
        std::cout << "Tree:\n";
        table->print_tree();
    }
    else {
        return MetaCommandResult::UNRECOGNIZED_COMMAND;
    }
}
