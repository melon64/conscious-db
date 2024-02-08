#ifndef ROW_H
#define ROW_H

#include <string>
#include <cstdint>
#include <cstring>

class Row {
public:
    static const size_t USERNAME_SIZE = 32;
    static const size_t EMAIL_SIZE = 255;

    Row() : id(0), username{0}, email{0} {}

    uint32_t get_id() const { return id; }
    void set_id(uint32_t id) { this->id = id; }

    const char* get_username() const { return username; }
    void set_username(const char* username) { strncpy(this->username, username, USERNAME_SIZE - 1); this->username[USERNAME_SIZE - 1] = '\0'; }

    const char* get_email() const { return email; }
    void set_email(const char* email) { strncpy(this->email, email, EMAIL_SIZE - 1); this->email[EMAIL_SIZE - 1] = '\0'; }

    void serialize(char* destination) const {
        memcpy(destination, &id, sizeof(id));
        memcpy(destination + sizeof(id), username, USERNAME_SIZE);
        memcpy(destination + sizeof(id) + USERNAME_SIZE, email, EMAIL_SIZE);
    }

    void deserialize(const char* source) {
        memcpy(&id, source, sizeof(id));
        memcpy(username, source + sizeof(id), USERNAME_SIZE);
        memcpy(email, source + sizeof(id) + USERNAME_SIZE, EMAIL_SIZE);
    }

private:
    uint32_t id;
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];
};

#endif