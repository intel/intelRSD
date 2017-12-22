/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 *
 * @brief Target LUN
 * */
#pragma once

#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! TargetLUN */
class TargetLun {
public:

    explicit TargetLun();
    TargetLun(const std::uint32_t lun,
              const std::string& logical_drive):
              m_lun{lun},m_logical_drive{logical_drive}
              {}

    ~TargetLun();

    /*! Enable copy */
    TargetLun(const TargetLun&) = default;
    TargetLun& operator=(const TargetLun&) = default;
    TargetLun(TargetLun&&) = default;
    TargetLun& operator=(TargetLun&&) = default;

    /*!
     * @brief setter for lun attribute
     *
     * @param lun of type std::uint32_t
     */
    void set_lun(const std::uint32_t lun) {
        m_lun = lun;
    }

    /*!
     * @brief return lun of type std::uint32_t
     *
     * @return the lun value
     */
    std::uint32_t get_lun() const {
        return m_lun;
    }

    /*!
     * @brief setter for logical_drive attribute
     *
     * @param logical_drive of type std::string
     */
    void set_logical_drive(const std::string& logical_drive) {
        m_logical_drive = logical_drive;
    }


    /*!
     * @brief return logical_drive of type const std::string&
     *
     * @return the logical_drive value
     */
    const std::string& get_logical_drive() const {
        return m_logical_drive;
    }

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class TargetLun from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed TargetLun object
     */
    static TargetLun from_json(const json::Json& json);

private:

    std::uint32_t m_lun{};
    std::string m_logical_drive{};

};

}
}
}
