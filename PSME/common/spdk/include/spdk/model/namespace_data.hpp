/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file namespace_data.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

class NamespaceData {
public:

    static constexpr const char ID[] = "id";


    /*!
     * @brief Gets the ID of Ns data
     * @return ID of Ns data
     */
    std::uint64_t get_id() const {
        return m_id;
    }


    /*!
     * @brief Sets the Ns data ID
     * @param id new ID of Ns data
     */
    void set_id(std::uint64_t id) {
        m_id = id;
    }


    /*!
     * @brief Lists current info about Ns data
     * @return info about Ns data as a string
     */
    std::string to_string() const;


private:
    std::uint64_t m_id{};

};


/*!
 * @brief converts from JSON to NamespaceData
 * @param json json object
 * @param ns_data NamespaceData object
 */
void from_json(const json::Json& json, NamespaceData& ns_data);


/*!
 * @brief converts from NamespaceData to Json
 * @param json Json object
 * @param ns_data NamespaceData object
 */
void to_json(json::Json& json, const NamespaceData& ns_data);

}
}



