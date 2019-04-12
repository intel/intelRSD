/*!
 * @brief Definition of Database virtual class.
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
 * @file database.hpp
 */

#pragma once

#include "agent-framework/module/utils/uuid.hpp"

#include <string>
#include <vector>
#include <memory>

namespace agent {
namespace nvme {
namespace tools {

class Database {
public:

    using Ptr = std::shared_ptr<Database>;

    enum class Types {
        ZONE,
        FABRIC,
        ENDPOINT,
        VOLUME,
        CHASSIS
    };

    /*!
     * @brief Destructor
     */
    virtual ~Database() {}

    /*!
     * @brief Store an attribute in database
     *
     * @param key Attribute's key
     * @param value Attribute's value
     */
    virtual void put(const std::string& key, const std::string& value) = 0;

    /*!
     * @brief Get an attribute from database
     *
     * @param key Attribute's key
     * @return Attribute's value
     */
    virtual std::string get(const std::string& key) const = 0;

    /*!
     * @brief Delete an attribute from database
     *
     * @param key Attribute's key
     */
    virtual void del(const std::string& key) = 0;

    /*!
     * @brief Append an entry to the list in given bucket
     *
     * @param bucket Bucket's name
     * @param value entry value
     */
    virtual void append(const std::string& bucket, const std::string& value) = 0;

    /*!
     * @brief Get all entries from given bucket
     *
     * @param bucket Bucket's name
     * @return vector of values from given bucket
     */
    virtual std::vector<std::string> get_multiple_values(const std::string& bucket) const = 0;

    /*!
     * @brief Remove database
     */
    virtual void remove() = 0;

};


}
}
}

