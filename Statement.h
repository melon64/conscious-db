#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>
#include <string>

using namespace std;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;
typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;


class Statement {
    public:
        StatementType type_;
        PrepareResult prepare_statement(string input) {
            if (input.substr(0, 6) == "insert") {
                type_ = STATEMENT_INSERT;
                return PREPARE_SUCCESS;
            }
            if (input.substr(0, 6) == "select") {
                type_ = STATEMENT_SELECT;
                return PREPARE_SUCCESS;
            }
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        void execute_statement() {
            switch (type_) {
                case STATEMENT_INSERT:
                    cout << "insert\n";
                    break;
                case STATEMENT_SELECT:
                    cout << "select\n";
                    break;
            }
        }
};

#endif