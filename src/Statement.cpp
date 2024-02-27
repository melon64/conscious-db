#include "Statement.h"
#include <sstream>

Statement::Statement() : type_(StatementType::INSERT) {}

PrepareResult Statement::prepare_statement(const std::string& input) {
    if (input.substr(0, 6) == "insert") {
        type_ = StatementType::INSERT;
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
            return PrepareResult::SYNTAX_ERROR;
        }
        if (username.length() > USERNAME_SIZE) {
            return PrepareResult::STRING_TOO_LONG;
        }
        if (email.length() > EMAIL_SIZE) {
            return PrepareResult::STRING_TOO_LONG;
        }
        if (id < 0) {
            return PrepareResult::NEGATIVE_ID;
        }

        row_.set_id(id);
        row_.set_username(username.c_str());
        row_.set_email(email.c_str());

        return PrepareResult::SUCCESS;
    }
    if (input.substr(0, 6) == "select") {
        type_ = StatementType::SELECT;
        return PrepareResult::SUCCESS;
    }
    return PrepareResult::UNRECOGNIZED_STATEMENT;
}

ExecuteResult Statement::execute_statement(Table* table) {
    switch (type_) {
    case StatementType::INSERT:
        return execute_insert(table);
    case StatementType::SELECT:
        return execute_select(table);
    }
    return ExecuteResult::SUCCESS;
}

ExecuteResult Statement::execute_insert(Table* table) {
    if (table->insert(row_)) {
        return ExecuteResult::SUCCESS;
    } else {
        return ExecuteResult::TABLE_FULL;
    }
}

ExecuteResult Statement::execute_select(Table* table) {
    table->select();
    return ExecuteResult::SUCCESS;
}
