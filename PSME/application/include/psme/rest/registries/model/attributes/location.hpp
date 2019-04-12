/*!
 * @brief Location class definition
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file location.hpp
 * */

#pragma once



#include "agent-framework/module/utils/utils.hpp"

namespace psme {
namespace rest {
namespace registries {

/*!
 * Class representing message registry file location.
 * */
class Location {
public:

    /*!
     * Set language of the resource pointed by location object.
     *
     * @param[in] language Resource language.
     * */
    void set_language(const OptionalField<std::string>& language);


    /*!
     * Get language of the resource pointed by location object.
     *
     * @return Resource language.
     * */
    const OptionalField<std::string>& get_language() const;


    /*!
     * Set URI of the resource pointed by location object.
     *
     * @param[in] uri Resource URI.
     * */
    void set_uri(const OptionalField<std::string>& uri);


    /*!
     * Get URI of the resource pointed by location object.
     *
     * @return Resource URI.
     * */
    const OptionalField<std::string>& get_uri() const;


    /*!
     * Set archive URI of the resource pointed by location object.
     *
     * @param[in] archive_uri Resource archive URI.
     * */
    void set_archive_uri(const OptionalField<std::string>& archive_uri);


    /*!
     * Get archive URI of the resource pointed by location object.
     *
     * @return Resource archive URI.
     * */
    const OptionalField<std::string>& get_archive_uri() const;


    /*!
     * Set publication URI of the resource pointed by location object.
     *
     * @param[in] publication_uri Resource publication URI.
     * */
    void set_publication_uri(const OptionalField<std::string>& publication_uri);


    /*!
     * Get publication URI of the resource pointed by location object.
     *
     * @return Resource publication URI.
     * */
    const OptionalField<std::string>& get_publication_uri() const;


    /*!
     * Set archive file of the resource pointed by location object.
     *
     * @param[in] archive_file Resource publication URI.
     * */
    void set_archive_file(const OptionalField<std::string>& archive_file);


    /*!
     * Get archive file of the resource pointed by location object.
     *
     * @return Resource publication URI.
     * */
    const OptionalField<std::string>& get_archive_file() const;


    /*!
     * Create a location object from a json::Json object.
     *
     * @param[in] json json::Json object to convert to Location object.
     *
     * @return Constructed Location object.
     * */
    static Location from_json(const json::Json& json);


private:
    std::string m_id{};
    OptionalField<std::string> m_name{};
    OptionalField<std::string> m_description{};
    OptionalField<std::string> m_language{};
    OptionalField<std::string> m_uri{};
    OptionalField<std::string> m_archive_uri{};
    OptionalField<std::string> m_publication_uri{};
    OptionalField<std::string> m_archive_file{};
};

}
}
}
