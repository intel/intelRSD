/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file file.cpp
 */

#include "nvmfpp/file.hpp"


#include <stdexcept>


nvmfpp::File::~File() {}

nvmfpp::File::File(const std::string& path, FileMode mode) {
    switch (mode) {
        case FileMode::READ:
            m_file.open(path, std::ios_base::in);
            break;
        case FileMode::WRITE:
            m_file.open(path, std::ios_base::out);
            break;
        case FileMode::READWRITE:
            m_file.open(path, std::ios_base::in | std::ios_base::out);
            break;
        default:
            throw std::runtime_error("Not supported mode.");
    }

}

std::string nvmfpp::File::read_from_file() {
    std::string content{};

    if (m_file.is_open()) {
        try {
            std::getline(m_file, content);
            return content;
        }
        catch (const std::exception& e) {
            throw std::runtime_error(std::string("Unable to read from file: ") + e.what());
        }
    }
    else {
        throw std::runtime_error("Could not open file.");
    }
}

void nvmfpp::File::write_to_file(const std::string& content) {
    if (m_file.is_open()) {
        m_file << content;
    }
    else {
        throw std::runtime_error("Could not open file.");
    }
}
