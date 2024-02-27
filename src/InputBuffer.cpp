#include "InputBuffer.h"

InputBuffer* InputBuffer::input_buffer_ = nullptr;

InputBuffer::InputBuffer() : buffer_length_(0), input_length_(0) {}

InputBuffer* InputBuffer::GetInstance() {
    if (input_buffer_ == nullptr) {
        input_buffer_ = new InputBuffer();
    }
    return input_buffer_;
}

void InputBuffer::print_prompt() {
    std::cout << "db > ";
}

void InputBuffer::read_input(std::istream& input) {
    std::getline(input, buffer_);
    input_length_ = buffer_.size();

    if (input_length_ <= 0) {
        std::cout << "Error reading input\n";
        std::exit(EXIT_FAILURE);
    }
}

void InputBuffer::close_input_buffer() {
    delete input_buffer_;
    input_buffer_ = nullptr;
}

std::string InputBuffer::get_input() {
    return buffer_;
}
