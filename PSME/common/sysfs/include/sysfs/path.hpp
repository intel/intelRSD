/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file sysfs/path.hpp
 */

#pragma once



#include <string>
#include <list>
#include <iosfwd>
#include <stdexcept>
#include <iterator>
#include <type_traits>



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
    Path(const char* str) : Path(std::string{str}) {}


    /*!
     * @brief Constructs a path based on the provided string
     * @param str String representation of the path
     */
    Path(char* str) : Path(static_cast<const char*>(str)) {}


    /*!
     * @brief Constructs a path based on the provided segments convertible to string
     * @param segment String representation of the path segment
     */
    template<typename Segment, typename... Segments>
    Path(Segment segment, Segments... segments) : Path(segments...) {

        static_assert(std::is_convertible<Segment, std::string>::value, "Path segment must be convertible to string");
        prepend(std::string(segment));
    }



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
     * @brief Returns the basename of the path (rightmost part of the path)
     * @return Basename of the path, "" for empty paths
     */
    std::string basename() const;


    /*!
     * @brief Returns the dirname from the path (everything but the rightmost part of the part)
     * @return Dirname of the path
     */
    Path dirname() const;


    /*!
     * @brief Removes the rightmost path element
     * @return Reference to the path
     */
    Path& pop_right();


    /*!
     * @brief Removes the leftmost path element
     * @return Reference to the path
     */
    Path& pop_left();


    /*!
     * @brief Appends relative path to another path
     * @param path Relative path to be appended
     * @return Reference to the path
     * @throws std::logic_error if resulting path is invalid
     */
    Path& append(const Path& path);


    /*!
     * @brief Prepends path to another path
     * @param path Path to be prepended
     * @return Reference to the path
     * @throws std::logic_error if resulting path is invalid
     */
    Path& prepend(const Path& path);


    /*!
     * @brief Returns i-th element of the path (0 for the leftmost element)
     * @param i Index of the element to be retrieved
     * @return i-th element of the path
     */
    const std::string& at(std::size_t i) const;


    /*!
     * @brief Returns number of elements in the path
     * Return Number of elements in the path
     */
    std::size_t size() const {
        return m_list.size();
    }


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


    /*!
     * @brief Returns true if path starts with the path provided
     * @param path Path to be checked for
     * @return False if path does not start with the provided path or if is of different type (absolute/negative)
     */
    bool starts_with(const Path& path) const;


    /*! @brief Works the same way as push */
    Path& operator<<(const Path& path) {
        return this->append(path);
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

}
