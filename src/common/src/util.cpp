#include "windows_fido_bridge/util.hpp"

#include "windows_fido_bridge/format.hpp"

#include <iostream>

namespace wfb {

namespace {

constexpr const size_t DUMP_BINARY_LINE_LENGTH = 16;

template <typename Output>
void dump_binary_line(Output& output, const uint8_t* buffer, size_t length) {
    // Print hex values
    for (size_t i = 0; i < length; i++) {
        output << "{:02x} "_format(buffer[i]);
    }

    // If we didn't print a full line, print some padding to line up this
    // partial line's ASCII output with the full lines above it
    if (length < DUMP_BINARY_LINE_LENGTH) {
        output << std::string((DUMP_BINARY_LINE_LENGTH - length) * 3, ' ');
    }

    output << ' ';

    // Print ASCII values
    for (size_t i = 0; i < length; i++) {
        char c = buffer[i];
        if (c < 0x20 || c > 0x7e) {
            // Non-printable ASCII, just print a placeholder
            c = '.';
        }

        output << c;
    }

    output << "\n";
}

}  // namespace

void dump_binary(std::stringstream& ss, const uint8_t* buffer, size_t length, size_t indent) {
    std::string indent_str(indent, ' ');

    // Printer a header
    ss << indent_str << "      ";
    for (size_t i = 0; i < DUMP_BINARY_LINE_LENGTH; i++) {
        ss << " {:x} "_format(i);
    }

    ss << "\n";

    // Print the values
    for (size_t i = 0; i < length; i += DUMP_BINARY_LINE_LENGTH) {
        ss << indent_str << "{:04x}: "_format(i);

        dump_binary_line(ss, buffer + i, std::min(DUMP_BINARY_LINE_LENGTH, length - i));
    }
}

std::tuple<uint8_t*, size_t> calloc_from_data(const uint8_t* buffer, size_t size) {
    uint8_t* result = reinterpret_cast<uint8_t*>(calloc(1, size));
    memcpy(result, buffer, size);
    return {result, size};
}

std::tuple<uint8_t*, size_t> calloc_from_data(const char* buffer, size_t size) {
    return calloc_from_data(reinterpret_cast<const uint8_t*>(buffer), size);
}

std::tuple<uint8_t*, size_t> calloc_from_data(const byte_vector& buffer) {
    return calloc_from_data(buffer.data(), buffer.size());
}

std::tuple<uint8_t*, size_t> calloc_from_data(const std::string& buffer) {
    return calloc_from_data(buffer.data(), buffer.size());
}

std::optional<std::string> get_environment_variable(const std::string& variable_name) {
    return get_environment_variable(variable_name.c_str());
}

std::optional<std::string> get_environment_variable(const char* variable_name) {
    const char* env_var_value = std::getenv(variable_name);
    if (env_var_value == nullptr) {
        return std::nullopt;
    }

    return std::string(env_var_value);
}

}  // namespace wfb
