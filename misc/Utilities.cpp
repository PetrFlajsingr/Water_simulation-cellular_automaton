//
// Created by Igor Frank on 05.12.19.
//

#include <fstream>
#include "Utilities.h"


std::string Utilities::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::string buffer(fileSize, ' ');

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}