#include <iostream>
#include <string>
#include <vector>

#include "InputBuffer.h"

using namespace std;

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = InputBuffer::GetInstance();
    while (true) {
        input_buffer->print_prompt();
        input_buffer->read_input();

        if (input_buffer->buffer_ == ".exit") {
        input_buffer->close_input_buffer();
        exit(EXIT_SUCCESS);
        } 
        else {
            cout << "Unrecognized command '" << input_buffer->buffer_ << "'.\n";
        }
    }
}
