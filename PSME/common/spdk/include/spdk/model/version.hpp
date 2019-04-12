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
 * @file version.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

class Version {
public:

    static constexpr const char NVME_VERSION[] = "nvme_version";


    /*!
     * @brief Gets the NVMe version
     * @return version of NVMe
     */
    const std::string& get_version() const {
        return m_version;
    }


    /*!
     * @brief Sets the NVMe version
     * @param version new version of NVMe
     */
    void set_version(const std::string& version) {
        m_version = version;
    }


    /*!
     * @brief Lists current info about NVMe version
     * @return info about NVMe version as a string
     */
    std::string to_string() const;


private:
    std::string m_version{};

};


/*!
 * @brief converts from JSON to Version
 * @param json json object
 * @param version Version object
 */
void from_json(const json::Json& json, Version& version);


/*!
 * @brief converts from Version to Json
 * @param json Json object
 * @param version Version object
 */
void to_json(json::Json& json, const Version& version);

}
}



