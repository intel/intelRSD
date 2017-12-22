/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @file sysfs/path.hpp
 */

#pragma once

#include <string>
#include <list>
#include <iosfwd>

namespace sysfs {

/*!
 * @brief Tool class for path handling.
 *
 * Responsible for a proper construction, merging and normalization of the paths. Handles absolute and
 * relative paths and allows path merging (using '/' operator). To be used as a simple path wrapper for
 * files, directories and symbolic links.
 */
class Path final {
public:

    /*!
     * @brief Constructs an empty relative path
     */
    Path() {}

    /*!
     * @brief Constructs a path based on the provided string
     * @param str String representation of the path
     */
    Path(const std::string& str);

    /*!
     * @brief Constructs a path based on the provided string
     * @param str String representation of the path
     */
    Path(const char* str): Path(std::string{str}) {}

    /*!
     * @brief Constructs the root absolute path object
     * @return Constructed object
     */
    static Path root();

    ~Path();

    /*! Enable copy */
    Path(const Path&) = default;
    Path& operator=(const Path&) = default;

    /*! Enable move */
    Path(Path&&) = default;
    Path& operator=(Path&&) = default;

    /*!
     * @brief Returns the basename of the path
     * @return Basename of the path, "" for empty paths
     */
    std::string basename() const;

    /*!
     * @brief Removes the top path element
     * @return Reference to the path
     */
    Path& pop_right();

    /*!
     * @brief Adds relative path to another path
     * @param path Relative path to be added
     * @return Reference to the path
     * @throws std::logic_error if resulting path is invalid
     */
    Path& push_right(const Path& path);

    /*!
     * @brief Checks if path is empty
     * @return True for empty paths
     */
    bool is_empty() const {
        return m_list.empty();
    }

    /*!
     * @brief Checks if path is an absolute path
     * @return True for absolute paths
     */
    bool is_absolute() const {
        return m_is_absolute;
    }

    /*!
     * @brief Checks if path is a relative path
     * @return True for relative paths
     */
    bool is_relative() const {
        return !m_is_absolute;
    }

    /*! @brief Works the same way as push */
    Path& operator<<(const Path& path) {
        return this->push_right(path);
    }

    /*! @brief Works the same way as to_string */
    operator std::string() const {
        return to_string();
    }

    /*!
     * @brief Returns a string representation of the path
     * @return Path converted to a string
     */
    std::string to_string() const;

private:

    /*!
     * @brief Rebuilds the path so it stores a 'normalized' path
     *        e.g "/not/../a//normalized/./path/" -> "/a/normalized/path"
     */
    void normalize();

    /*!
     * @brief Constructs path from a string
     */
    static Path from_string(const std::string& str);

    bool m_is_absolute{false};
    std::list<std::string> m_list{};

};

}

/*!
 * @brief Used to merge path with another relative path
 * @param lhs Relative or absolute path
 * @param rhs Relative path
 * @return Merged path
 * @throws std::logic_error if invalid path is to be created
 */
sysfs::Path operator/(const sysfs::Path& lhs, const sysfs::Path& rhs);

/*!
 * @brief Compares to paths
 * @param lhs First path to compare
 * @param rhs Second path to compare
 * @return True if two paths are the same
 */
bool operator==(const sysfs::Path& lhs, const sysfs::Path& rhs);

/*!
 * @brief Compares to paths
 * @param lhs First path to compare
 * @param rhs Second path to compare
 * @return True if two paths are different
 */
bool operator!=(const sysfs::Path& lhs, const sysfs::Path& rhs);

/*!
 * @brief Put string representation of a Path in an std stream
 * @param os stream object
 * @param path the Path to print
 * @return the stream object
 */
std::ostream& operator<<(std::ostream& os, const sysfs::Path& path);
