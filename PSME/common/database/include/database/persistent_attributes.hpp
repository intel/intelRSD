/*!
 * @brief Class to keep all users' settings done via API
 *
 * All settings are kept persistently by the database.
 *
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
 * @file config/persistent_attributes.hpp
 */

#pragma once

#include "database/database.hpp"

#include <mutex>
#include <utility>
#include <map>

namespace database {

class PersistentAttributes final {
public:
    using UuidAttrPair = std::pair<std::string, std::string>;
    using UuidAttrVector = std::vector<UuidAttrPair>;

    /*!
     * @brief Get instance of configuration user settings.
     * @return singleton instance
     */
    static PersistentAttributes& get_instance();

    /*!
     * @brief Get value from the database or assume default one
     * @param uuid uuid of resource
     * @param attr attribute with requested value
     * @param[out] ret_val value kept by the database
     * @return true if attribtue is set
     */
    bool get_value(const std::string& uuid, const std::string& attr, std::string& ret_val);

    /*!
     * @brief Set value (persistently in the database) to be reported by discovery
     * @param uuid uuid of resource
     * @param attr attribute to set a value
     * @param value requested value
     */
    void set_value(const std::string& uuid, const std::string& attr, const std::string& value);

    /*!
     * @brief Remove value
     * @param uuid uuid of resource
     * @param attr attribute name
     */
    void remove_value(const std::string& uuid, const std::string& attr);

    /*!
     * @brief Get all entry keys from the database
     * @return vector with all keys (uuid, attr) from the database
     */
    UuidAttrVector get_persistent_settings();

private:
    /*!
     * @brief Default constructor.
     */
    PersistentAttributes();

    /*!
     * @brief Default destructor.
     */
    ~PersistentAttributes();

    /*!
     * @brief Database to keep all the settings persistently
     */
    database::Database::SPtr db{};

    using UuidAttrValueMap = std::map<UuidAttrPair, std::string>;
    UuidAttrValueMap settings{};

    mutable std::mutex mutex{};
};

}
