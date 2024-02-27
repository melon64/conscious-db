#ifndef META_COMMAND_H
#define META_COMMAND_H

#include <string>

#include "Table.h"

enum class MetaCommandResult { SUCCESS, UNRECOGNIZED_COMMAND };

class MetaCommand {
public:
    MetaCommandResult execute_meta_command(std::string input, Table* table);
};

#endif
