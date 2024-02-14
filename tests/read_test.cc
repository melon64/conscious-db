#include <gtest/gtest.h>

#include "InputBuffer.h"

TEST(InitialReadWrite, ReadInput) {
    InputBuffer *input_buffer = InputBuffer::GetInstance();
    input_buffer->buffer_ = "test";
    input_buffer->input_length_ = 4;
    ASSERT_EQ(input_buffer->get_input(), "test");
}
