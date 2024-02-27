#include "Row.h"
#include <cstring>

Row::Row() : id(0) {
    std::memset(username, 0, sizeof(username));
    std::memset(email, 0, sizeof(email));
}

size_t Row::get_id() const {
    return id;
}

void Row::set_id(size_t id) {
    this->id = id;
}

const char* Row::get_username() const {
    return username;
}

void Row::set_username(const char* username) {
    std::strncpy(this->username, username, USERNAME_SIZE);
    this->username[USERNAME_SIZE] = '\0';
}

const char* Row::get_email() const {
    return email;
}

void Row::set_email(const char* email) {
    std::strncpy(this->email, email, EMAIL_SIZE);
    this->email[EMAIL_SIZE] = '\0';
}

void Row::serialize(char* destination) const {
    std::memcpy(destination, &id, sizeof(id));
    std::memcpy(destination + sizeof(id), username, sizeof(username));
    std::memcpy(destination + sizeof(id) + sizeof(username), email, sizeof(email));
}

void Row::deserialize(const char* source) {
    std::memcpy(&id, source, sizeof(id));
    std::memcpy(username, source + sizeof(id), sizeof(username));
    std::memcpy(email, source + sizeof(id) + sizeof(username), sizeof(email));
}
