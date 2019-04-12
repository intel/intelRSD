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
 * @brief Manager specific data used in commands response
 * */

#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief Class representing elements of Mangers arrays */
class ManagerEntry {
public:
    explicit ManagerEntry();

    explicit ManagerEntry(const std::string& manager) : m_manager(manager) {}

    ~ManagerEntry();

    /*! Enable copy */
    ManagerEntry(const ManagerEntry&) = default;
    ManagerEntry& operator=(const ManagerEntry&) = default;
    ManagerEntry(ManagerEntry&&) = default;
    ManagerEntry& operator=(ManagerEntry&&) = default;

    /*!
     * @brief setter for manager attribute
     *
     * @param manager of type std::string
     */
    void set_manager(const std::string& manager) {
        m_manager = manager;
    }

    /*!
     * @brief return manager of type const std::string&
     *
     * @return the manager value
     */
    const std::string& get_manager() const {
        return m_manager;
    }

    /*!
     * @brief Make Json from managers array entry
     * @return Json object
     */
    json::Json to_json() const;

    /*!
     * @brief make managers array entry from Json
     * @param[in] json the Json input
     * @return new collection entry
     */
    static ManagerEntry from_json(const json::Json& json);

private:

    std::string m_manager{};

};

}
}
}
