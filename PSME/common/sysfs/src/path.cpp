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
 * @file sysfs/path.cpp
 */

#include "sysfs/path.hpp"

#include <stdexcept>
#include <sstream>
#include <ostream>

using namespace sysfs;

Path::Path(const std::string& str) {
    *this = Path::from_string(str);
}

Path::~Path() {}

std::string Path::basename() const {
    if (is_empty()) {
        return {};
    }
    return m_list.front();
}

Path Path::dirname() const {
    return Path(*this).pop_right();
}

bool Path::starts_with(const Path& path) const {
    // different types of paths
    if (path.is_relative() != is_relative()) {
        return false;
    }
    // test path is longer than tested path
    if (path.m_list.size() > m_list.size()) {
        return false;
    }
    // check field by field
    for (auto it1 = m_list.crbegin(), it2 = path.m_list.crbegin(); it1 != m_list.crend() && it2 != path.m_list.crend(); ++it1, ++it2) {
        if (*it1 != *it2) {
            return false;
        }
    }
    return true;
}

Path& Path::pop_right() {
    if (!m_list.empty()) {
        m_list.pop_front();
    }
    return *this;
}

Path& Path::pop_left() {
    if (!m_list.empty()) {
        m_list.pop_back();
        m_is_absolute = false;
    }
    return *this;
}

Path& Path::append(const Path& path) {

    if (path.is_absolute()) {
        // cannot add absolute path
        throw std::logic_error(std::string{"Absolute path cannot be appended to another path: "} + to_string() + " + " + path.to_string());
    }

    for (auto it = path.m_list.crbegin(); it != path.m_list.crend(); ++it) {
        m_list.push_front(*it);
    }

    normalize();

    return *this;
}

Path& Path::prepend(const Path& path) {

    if (is_absolute()) {
        // cannot prepend an absolute path
        throw std::logic_error(std::string{"Absolute path cannot be prepended: "} + path.to_string() + " + " + to_string());
    }

    auto tmp = path;
    tmp.append(*this);
    *this = std::move(tmp);

    return *this;
}

std::string Path::to_string() const {
    std::string ret = (m_is_absolute ? std::string{"/"} : std::string{});

    for (auto it = m_list.crbegin(); it != m_list.crend();) {
        ret += *it;
        if (++it != m_list.crend()) {
            ret += "/";
        }
    }
    return ret;
}

Path Path::from_string(const std::string& str) {

    // empty string - return empty path
    if (str.empty()) {
        return Path{};
    }

    // handle absolute paths
    Path path{};
    std::istringstream stream(str);
    if (str[0] == '/') {
        path.m_is_absolute = true;
        stream.ignore(1);
    }

    // extract all elements from the path
    for (std::string name{}; std::getline(stream, name, '/'); ) {
        path.m_list.push_front(name);
    }

    path.normalize();

    return path;
}

Path Path::root() {
    Path path{};
    path.m_is_absolute = true;
    return path;
}

void Path::normalize() {
    std::list<std::string> canonical{};
    for (auto it = m_list.crbegin(); it != m_list.crend(); ++it) {

        // ignore "/./" and "//" terms in the path
        if (*it == "." || *it == "") {
            continue;
        }

        // add "/../" only if there is nothing to be taken from the path, otherwise, remove element
        if (*it == "..") {
            if (canonical.empty() || canonical.front() == "..") {
                // paths like "/.." have no sense, throws
                if (m_is_absolute) {
                    throw std::logic_error(std::string{"Path ("} + to_string() + ") is invalid");
                }
                canonical.push_front(*it);
            }
            else {
                canonical.pop_front();
            }
            continue;
        }

        // normal path - just add it
        canonical.push_front(*it);
    }
    m_list = canonical;
}

const std::string& Path::at(std::size_t i) const {
    if (i >= m_list.size()) {
        throw std::out_of_range("Index points outside the path");
    }
    auto it = m_list.crbegin();
    std::advance(it, i);
    return *it;
}

Path sysfs::operator/(const Path& lhs, const Path& rhs) {
    Path path = lhs;
    return (path << rhs);
}

bool sysfs::operator==(const sysfs::Path& lhs, const sysfs::Path& rhs) {
    return lhs.to_string() == rhs.to_string();
}

bool sysfs::operator!=(const sysfs::Path& lhs, const sysfs::Path& rhs) {
    return !(lhs == rhs);
}

std::ostream& sysfs::operator<<(std::ostream& os, const sysfs::Path& path) {
    return os << path.to_string();
}
