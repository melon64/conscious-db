#ifndef ROW_H
#define ROW_H

#include <string>

static const size_t USERNAME_SIZE = 32;
static const size_t EMAIL_SIZE = 255;

class Row {
public:
    Row();

    size_t get_id() const;
    void set_id(size_t id);

    const char* get_username() const;
    void set_username(const char* username);

    const char* get_email() const;
    void set_email(const char* email);

    void serialize(char* destination) const;
    void deserialize(const char* source);

private:
    size_t id;
    char username[USERNAME_SIZE + 1];
    char email[EMAIL_SIZE + 1];
};

#endif
