/*!
 * @section LICENSE
 *
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
 * */
#include "status/module_software_status.hpp"

using namespace logger_cpp;
using namespace agent::compute::status;
using Status = agent_framework::status::ModuleStatus::Status;

ModuleSoftwareStatus::~ModuleSoftwareStatus() {
}

Status ModuleSoftwareStatus::read_status() {
    Status status = Status::UNKNOWN;
    try {
        bool is_pong = m_icmp.ping();

        if (is_pong && !m_smbios_checksum.is_updated()) {
            status = Status::PRESENT;
        }
        else {
            if (!is_pong) {
                m_smbios_checksum = SmbiosChecksum{m_smbios_checksum.get_uuid()};
            }
            status = Status::NOT_PRESENT;
        }
    }
    catch (std::runtime_error& error) {
        log_warning(GET_LOGGER("status"), "Unable to read module software status: " << error.what() << std::endl);
        m_smbios_checksum = SmbiosChecksum{m_smbios_checksum.get_uuid()};
        status = Status::UNKNOWN;
    }
    m_status = status;
    return m_status;
}
