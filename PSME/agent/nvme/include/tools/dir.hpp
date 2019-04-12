/*!
 * @brief Definition of Dir class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file dir.hpp
 */

#pragma once

#include <string>
#include <vector>

namespace agent {
namespace nvme {
namespace tools {

class Dir {
public:
    /*!
     * @brief Read directories from given path
     *
     * @param path path where to look for directories
     *
     * @return vector of directories
     */
    virtual std::vector<std::string> read_directories(const std::string& path) const = 0;

    /*!
     * @brief Read symbolic links from given path
     *
     * @param path path where to look for the links
     *
     * @return vector of links
     */
    virtual std::vector<std::string> read_links(const std::string& path) const = 0;

    /*!
     * @brief Read first line from a file
     *
     * @param path path to a file
     *
     * @return line
     */
    virtual std::string read_first_line(const std::string& path) const = 0;
};

}
}
}

