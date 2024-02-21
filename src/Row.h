#ifndef ROW_H
#define ROW_H

#include <string>
#include <cstdint>
#include <cstring>

static const size_t USERNAME_SIZE = 32;
static const size_t EMAIL_SIZE = 255;

class Row {
public:
    Row() : id(0), username{0}, email{0} {}

    size_t get_id() const { return id; }
    void set_id(size_t id) { this->id = id; }

    const char* get_username() const { return username; }
    void set_username(const char* username) {
        strncpy(this->username, username, USERNAME_SIZE);
        this->username[USERNAME_SIZE] = '\0';
    }

    const char* get_email() const { return email; }
    void set_email(const char* email) {
        strncpy(this->email, email, EMAIL_SIZE);
        this->email[EMAIL_SIZE] = '\0';
    }

    void serialize(char* destination) const {
        memcpy(destination, &id, sizeof(id));
        memcpy(destination + sizeof(id), username, sizeof(username));
        memcpy(destination + sizeof(id) + sizeof(username), email, sizeof(email));
    }

    void deserialize(const char* source) {
        memcpy(&id, source, sizeof(id));
        memcpy(username, source + sizeof(id), sizeof(username));
        memcpy(email, source + sizeof(id) + sizeof(username), sizeof(email));
    }

private:
    size_t id;
    char username[USERNAME_SIZE+1];
    char email[EMAIL_SIZE+1];
};

#endif