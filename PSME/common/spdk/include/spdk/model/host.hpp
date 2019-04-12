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
 * @file host.hpp
 */
#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

class Host {
public:

    static constexpr const char NQN[] = "nqn";


    /*!
     * @brief Gets the Host's NQN
     * @return Host NQN as string
     */
    const std::string& get_nqn() const {
        return m_nqn;
    }


    /*!
     * @brief Sets the Host's NQN
     * @param nqn host NQNs
     */
    void set_nqn(const std::string& nqn) {
        m_nqn = nqn;
    }


    /*!
     * @brief Lists current info about Host
     * @return info about Host as a string
     */
    std::string to_string() const;


private:
    std::string m_nqn{};

};


/*!
 * @brief converts from JSON to Host
 * @param json json object
 * @param host Host object
 */
void from_json(const json::Json& json, Host& host);


/*!
 * @brief converts from ListenAddress to Json
 * @param json Json object
 * @param host Host object
 */
void to_json(json::Json& json, const Host& host);

}
}


