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
 * @file file.hpp
 */

#pragma once


#include "base_file.hpp"


#include <fstream>


namespace nvmfpp {

class File final : public nvmfpp::BaseFile {
public:

    /*!
     * @brief Construct object of file.
     * @param path Path to file.
     * @param mode File access mode.
     */
    File(const std::string& path, FileMode mode);
    ~File();

    /*!
     * @brief Reads first line from file.
     * @return string First line of file.
     */
    virtual std::string read_from_file() override;

    /*!
     * @brief Writes to file in path. Method replace content in file.
     * @param content string which is going to be written to file.
     */
    virtual void write_to_file(const std::string& content) override;

private:
    std::fstream m_file{};

};

}
