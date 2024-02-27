#ifndef STATEMENT_H
#define STATEMENT_H

#include <string>
#include "Row.h"
#include "Table.h"

enum class StatementType { INSERT, SELECT };
enum class PrepareResult { SUCCESS, UNRECOGNIZED_STATEMENT, SYNTAX_ERROR, STRING_TOO_LONG, NEGATIVE_ID };
enum class ExecuteResult { SUCCESS, TABLE_FULL };

class Statement {
public:
    Statement();

    PrepareResult prepare_statement(const std::string& input);

    ExecuteResult execute_statement(Table* table);

private:
    StatementType type_;
    Row row_;

    ExecuteResult execute_insert(Table* table);
    ExecuteResult execute_select(Table* table);
};

#endif
