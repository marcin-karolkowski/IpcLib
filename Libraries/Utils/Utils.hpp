#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cinttypes>

#include "String.hpp"

namespace Utils::Utils
{
    std::string ReadTextFile(const std::string& path);
    std::vector<std::string> ReadTextFileLines(const std::string& path);
    void WriteTextFile(const std::string& path, const std::string& content);
    void WriteTextFileLines(const std::string& path, const std::vector<std::string>& lines);
}
#endif