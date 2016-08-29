#pragma once

#include "string_utils.hpp"

#include <iostream>
#include <string>

namespace tumbletest {

enum LogType {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

#define Info(...) tumbletest::Log<tumbletest::LogType::INFO>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))
#define Warning(...) tumbletest::Log<tumbletest::LogType::WARNING>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))
#define Error(...) tumbletest::Log<tumbletest::LogType::ERROR>(__FILE__, std::to_string(__LINE__), StrCat(__VA_ARGS__))

template<int>
void Log(const std::string& file, const std::string& line, const std::string& message);

template<>
void Log<LogType::INFO>(const std::string& file, const std::string& line, const std::string& message) {
    std::cerr << "[INFO]" << file << ":" << line << " > " << message << '\n';
}

template<>
void Log<LogType::WARNING>(const std::string& file, const std::string& line, const std::string& message) {
    std::cerr << "[WARNING]" << file << ":" << line << " > " << message << '\n';
}

template<>
void Log<LogType::ERROR>(const std::string& file, const std::string& line, const std::string& message) {
    std::cerr << "[ERROR]" << file << ":" << line << " > " << message << '\n';
    exit(0);
}

}  // namespace tumbletest