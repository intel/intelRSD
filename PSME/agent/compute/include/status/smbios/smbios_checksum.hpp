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
 * @file smbios_checksum.hpp
 * @brief Smbios checksum read implementation
*/

#pragma once

#include "agent-framework/module/model/attributes/connection_data.hpp"
#include <iostream>
#include <memory>

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

using agent_framework::model::attribute::ConnectionData;
using ConnectionDataSharedPtr = std::shared_ptr<ConnectionData>;
/*!
 * @brief SmbiosChecksum class.
 * */
class SmbiosChecksum final {
    std::string m_uuid{};
    uint8_t m_checksum{};
    ConnectionDataSharedPtr m_connection_data{};
    /*!
    * @brief Reads SMBIOS MDR data region checksum
    * @return SMBIOS MDR data region checksum
    */
    uint8_t read_checksum();
public:
    /*!
     * @brief Class default constructor.
     * @param uuid monitored Manager's uuid.
     * */
    SmbiosChecksum(const std::string& uuid) : m_uuid(uuid) {
    }

    /*!
     * @brief Return SMBIOS MDR data region update flag
     * @return true if SMBIOS MDR data region has been updated
     */
    bool is_updated();

    /*
     * Getter for monitored Manager's uuid.
     * @return Monitored Manager's uuid.
     */
    const std::string& get_uuid() const {
        return m_uuid;
    }
};

}
}
}
