#include <gtest/gtest.h>

#include "InputBuffer.h"
#include <iostream>

TEST(InitialReadWrite, ReadInput) {
    std::stringstream ss;
    ss << "test";
    InputBuffer *input_buffer = InputBuffer::GetInstance();
    input_buffer->read_input(ss);
    ASSERT_EQ(input_buffer->get_input(), "test"); //input "test" for cin
}

