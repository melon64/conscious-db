#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class InputBuffer {
    protected:
        InputBuffer(): buffer_length_(0), input_length_(0) {}
        static InputBuffer *input_buffer_;

    public:
        string buffer_;
        size_t buffer_length_;
        ssize_t input_length_;

        InputBuffer(const InputBuffer& other) = delete;
        InputBuffer& operator=(const InputBuffer& other) = delete;

        static InputBuffer *GetInstance() {
            if (input_buffer_ == nullptr) {
                input_buffer_ = new InputBuffer();
            }
            return input_buffer_;
        }

        void print_prompt() {
            cout << "db > ";
        }

        void read_input(istream& input) {
            getline(input, buffer_);
            input_length_ = buffer_.size();

            if (input_length_ <= 0) {
                cout << "Error reading input\n";
                exit(EXIT_FAILURE);
            }
        }

        void close_input_buffer() {
            delete input_buffer_;
            input_buffer_ = nullptr;
        }

        string get_input() {
            return buffer_;
        }
};

InputBuffer* InputBuffer::input_buffer_ = nullptr;

#endif