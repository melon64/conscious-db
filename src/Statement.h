#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include <string>
#include <sstream>
#include "Row.h"
#include "Table.h"

using namespace std;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;
typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT, PREPARE_SYNTAX_ERROR } PrepareResult;
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;


class Statement {
public:
    Statement() {}

    PrepareResult prepare_statement(string input) {
        if (input.substr(0, 6) == "insert") {
            type_ = STATEMENT_INSERT;
            int id;
            std::string username;
            std::string email;
            int args = 0;
            std::istringstream iss(input);
            std::string command;
            iss >> command;
            if (command == "insert") {
                iss >> id >> username >> email;
                args = (iss.fail() ? -1 : 3);
            }
            if (args < 3) {
                return PREPARE_SYNTAX_ERROR;
            }

            row_.set_id(id);
            row_.set_username(username.c_str());
            row_.set_email(email.c_str());

            return PREPARE_SUCCESS;
        }
        if (input.substr(0, 6) == "select") {
            type_ = STATEMENT_SELECT;
            return PREPARE_SUCCESS;
        }
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }

    ExecuteResult execute_insert(Statement *statement, Table *table) {
        if (table->insert(statement->row_)){
            return EXECUTE_SUCCESS;
        } else {
            return EXECUTE_TABLE_FULL;
        }
    }

    ExecuteResult execute_select(Statement *statement, Table *table) {
        table->select();
        return EXECUTE_SUCCESS;
    }

    ExecuteResult execute_statement(Table *table) {
        switch (type_) {
            case STATEMENT_INSERT:
                return execute_insert(this, table);
                break;
            case STATEMENT_SELECT:
                return execute_select(this, table);
                break;
        }
        return EXECUTE_SUCCESS;
    }

private:
    StatementType type_;
    Row row_;
};

#endif