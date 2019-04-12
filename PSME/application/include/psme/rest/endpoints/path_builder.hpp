/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file path_builder.hpp
 * */

#pragma once
#include "psme/rest/constants/constants.hpp"

#include <string>
#include <sstream>

namespace psme {
namespace rest {

namespace server {
    class Request;
}

namespace endpoint {

/*!
 * @brief Utility class for URL path creation.
 */
class PathBuilder {
public:
    /*! @brief Constructor */
    PathBuilder() = default;

    /*!
     * @brief Constructor
     * @param base Base path
     */
    template<class T>
    explicit PathBuilder(const T& base) {
        m_str << base;
    }

    /*!
     * @brief Constructor
     * @param request Request object, which get_url() (with trailing slash trimmed)
     *        is used as base path.
     */
    explicit PathBuilder(const psme::rest::server::Request& request);

    /*!
     * @brief Appends path segment to build path.
     *
     * Segment is preceded with PATH_SEP.
     *
     * @param segment Segment appended to build path. Segment is preceded with PATH_SEP.
     * @return PathBuilder reference for chaining.
     */
    template<class T>
    PathBuilder& append(T&& segment) {
        m_str << psme::rest::constants::PathParam::PATH_SEP
              << std::forward<T>(segment);
        return *this;
    }

    /*!
     * @brief Appends variable path segment to build path.
     *
     * ex. append_variable("var1") results in appending "/{var1}" segment to build path.
     *
     * @param name Variable name of segment appended to build path.
     * @return PathBuilder reference for chaining.
     */
    PathBuilder& append_variable(const std::string& name);

    /*!
     * @brief Appends variable path segment to build path.
     *
     * ex. append_regex("var1", "[0-9]+") results in appending "/{var1:[0-9]+}" segment
     * to build path.
     *
     * @param[in] name Variable name part of segment appended to build path.
     * @param[in] regex Regex part of segment appended to build path.
     * @return PathBuilder reference for chaining.
     */
    PathBuilder& append_regex(const std::string& name, const std::string& regex);

    /*!
     * @brief Appends jsonptr segment to build path.
     *
     * ex. append_jsonptr("/key/3") results in appending "#/key/3" segment to build path.
     *
     * @param[in] jsonptr Variable jsonptr segment appended to build path.
     * @return PathBuilder reference for chaining.
     */
    PathBuilder& append_jsonptr(const std::string& jsonptr);

    /*!
     * @brief Builds URL path.
     *
     * @return Resulting URL path.
     */
    std::string build();

private:
    std::ostringstream m_str{};
};


}
}
}

