#pragma once

#include <charconv>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

#include "models.hpp"


constexpr int DEFAULT_STEPS = 20;
constexpr int MAX_STEPS = 100000;

template <typename T>
bool parseValue(std::string_view text, T& value) {
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);

    return ec == std::errc() && ptr == end;
}


class Console {
public:
    Console(std::istream& in, std::ostream& out) : in_(in), out_(out) {}

    double readDouble(const std::string& name, double defaultValue);

    double readPositive(const std::string& name, double defaultValue);

    int readChoice(const std::string& prompt, int maxValue);

    int readSteps();

    std::unique_ptr<Model> createModel(int choice);

private:
    std::istream& in_;
    std::ostream& out_;
};
