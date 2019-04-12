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
 * @file controller_status.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

class ControllerStatus {
public:

    static constexpr const char RDY[] = "rdy";
    static constexpr const char CFS[] = "cfs";


    /*!
     * @brief Gets the Rdy
     * @return Rdy
     */
    std::uint64_t get_rdy() const {
        return m_rdy;
    }


    /*!
     * @brief Sets the Rdy
     * @param rdy new Rdy
     */
    void set_rdy(std::uint64_t rdy) {
        m_rdy = rdy;
    }


    /*!
     * @brief Gets the Cfs
     * @return Cfs
     */
    std::uint64_t get_cfs() const {
        return m_cfs;
    }


    /*!
     * @brief Sets the Cfs
     * @param cfs new Cfs
     */
    void set_cfs(std::uint64_t cfs) {
        m_cfs = cfs;
    }


    /*!
     * @brief Lists current info about Csts
     * @return info about Csts as a string
     */
    std::string to_string() const;


private:
    std::uint64_t m_rdy{};
    std::uint64_t m_cfs{};

};


/*!
 * @brief converts from JSON to ControllerStatus
 * @param json json object
 * @param csts ControllerStatus object
 */
void from_json(const json::Json& json, ControllerStatus& csts);


/*!
 * @brief converts from Csts to Json
 * @param json Json object
 * @param csts Csts object
 */
void to_json(json::Json& json, const ControllerStatus& csts);

}
}



