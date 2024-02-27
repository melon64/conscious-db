#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <iostream>
#include <string>
#include <vector>

class InputBuffer {
protected:
    InputBuffer();
    InputBuffer(const InputBuffer& other) = delete;
    InputBuffer& operator=(const InputBuffer& other) = delete;

public:
    static InputBuffer* GetInstance();

    void print_prompt();

    void read_input(std::istream& input);

    void close_input_buffer();

    std::string get_input();

private:
    std::string buffer_;
    size_t buffer_length_;
    ssize_t input_length_;

    static InputBuffer* input_buffer_;
};

#endif
