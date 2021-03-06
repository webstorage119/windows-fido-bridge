#include "windows_fido_bridge/communication.hpp"

#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>

namespace {

void read_all_from(int fd, uint8_t* data, size_t length);
void write_all_into(int fd, const uint8_t* buffer, size_t num_bytes);

}  // namespace

namespace wfb {

void send_message(int fd, std::string_view message) {
    send_message(fd, message.data(), message.size());
}

void send_message(int fd, const byte_vector& message) {
    send_message(fd, message.data(), message.size());
}

void send_message(int fd, const char* message_buffer, size_t message_length) {
    send_message(fd, reinterpret_cast<const uint8_t*>(message_buffer), message_length);
}

void send_message(int fd, const uint8_t* message_buffer, size_t message_length) {
    // Write the message header
    std::stringstream header_ss;
    header_ss << message_length << "|";
    std::string header = header_ss.str();
    write_all_into(fd, reinterpret_cast<const uint8_t*>(header.data()), header.size());

    // Write the message data
    write_all_into(fd, message_buffer, message_length);
}

byte_vector receive_message(int fd) {
    // Read the message header
    size_t message_size = 0;
    while (true) {
        unsigned char size_char = 0;
        read_all_from(fd, &size_char, 1);

        if (size_char == '|') {
            break;
        }

        if (size_char < '0' || size_char > '9') {
            throw std::runtime_error("Failed to reach message header");
        }

        message_size = message_size * 10 + (size_char - '0');
        if (message_size > 32768) {
            throw std::runtime_error("Message too large");
        }
    }

    // Read the message data
    byte_vector message(message_size, 0);
    read_all_from(fd, message.data(), message.size());
    return message;
}

}  // namespace wfb

namespace {

void read_all_from(int fd, uint8_t* data, size_t length) {
    ssize_t num_read = 0;
    while (num_read < length) {
        ssize_t result = read(fd, data + num_read, length - num_read);

        if (result == 0) {
            throw std::runtime_error("End of stream");
        }

        if (result == -1) {
            throw std::runtime_error(strerror(errno));
        }

        num_read += result;
    }
}

void write_all_into(int fd, const uint8_t* data, size_t length) {
    ssize_t num_written = 0;
    while (num_written < length) {
        ssize_t result = write(fd, data + num_written, length - num_written);
        if (result == -1) {
            throw std::runtime_error(strerror(errno));
        }

        num_written += result;
    }
}

}  // namespace
