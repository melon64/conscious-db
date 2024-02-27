#include "MetaCommand.h"

MetaCommandResult MetaCommand::execute_meta_command(std::string input, Table* table) {
    if (input == ".exit") {
        table->db_close();
        std::exit(EXIT_SUCCESS);
    } else {
        return MetaCommandResult::UNRECOGNIZED_COMMAND;
    }
}
