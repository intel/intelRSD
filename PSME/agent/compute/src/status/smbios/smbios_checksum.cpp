/*!
 * @section LICENSE
 *
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
 * */
#include "status/smbios/smbios_checksum.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/status/module_status.hpp"
#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "make_unique.hpp"



using namespace logger_cpp;
using namespace agent::compute::status;
using namespace future;
using namespace ipmi;
using namespace ipmi::command;
using namespace agent_framework::generic;
using namespace agent_framework::model;
using namespace agent_framework::module;
using Status = agent_framework::status::ModuleStatus::Status;
using ManagementControllerUnique = std::unique_ptr<ipmi::ManagementController>;
using ManagerRef = ObjReference<Manager, std::recursive_mutex>;

namespace {
ManagerRef get_manager(const std::string& uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();

    if (!module_manager.entry_exists(uuid)) {
        throw std::runtime_error("Manager: " + uuid + " does not exist.");
    }
    return module_manager.get_entry_reference(uuid);
}


ConnectionDataSharedPtr get_connection_data(const std::string& uuid) {
    auto manager = get_manager(uuid);
    if (!manager->get_presence()) {
        throw std::runtime_error("Manager: " + uuid + " is not present yet. Skipping.");
    }
    return std::make_shared<agent_framework::model::attribute::ConnectionData>(manager->get_connection_data());
}


void set_ipmi_connection_data(ManagementControllerUnique& mc, ConnectionDataSharedPtr connection_data) {
    mc->set_ip(connection_data->get_ip_address());
    mc->set_port(connection_data->get_port());
    mc->set_username(connection_data->get_username());
    mc->set_password(connection_data->get_password());
}


bool is_valid(sdv::response::GetMdrDataRegionStatus& response) {
    if (response.get_lock_status() != ipmi::command::sdv::LockStatus::UNLOCKED) {
        log_warning("smbios-discovery", "MDR Region locked!");
        return false;
    }
    else if (response.get_data_validation() != ipmi::command::sdv::DataValidation::VALID) {
        log_warning("smbios-discovery", "MDR Region not valid!");
        return false;
    }
    else if (response.get_region_size_used() == 0) {
        log_warning("smbios-discovery", "MDR Region is empty!");
        return false;
    }
    return true;
}
}


uint8_t SmbiosChecksum::read_checksum() {
    if (!m_connection_data) {
        m_connection_data = get_connection_data(m_uuid);
    }
    std::unique_ptr<ipmi::ManagementController> mc = make_unique<ipmi::manager::ipmitool::ManagementController>();
    set_ipmi_connection_data(mc, m_connection_data);

    sdv::request::GetMdrDataRegionStatus request{};
    sdv::response::GetMdrDataRegionStatus response{};

    request.set_data_region_id(sdv::DataRegionId::SMBIOS_TABLE);

    try {
        mc->send(request, response);
        if (is_valid(response)) {
            return response.get_region_checksum();
        }
    }
    catch (const ipmi::ResponseError& error) {
        log_error("smbios-discovery", " GetMdrDataRegionStatus command error.");
        log_error("compute-discovery",
                  "GetMdrDataRegionStatus - bad IPMI completion code: " << uint32_t(error.get_completion_code()));
    }

    throw std::runtime_error("Read checksum for manager: " + m_uuid + " failed. " + std::to_string(int(m_checksum)));
}


bool SmbiosChecksum::is_updated() {
    uint8_t checksum = m_checksum;
    try {
        m_checksum = read_checksum();
        if (m_checksum != checksum) {
            log_info("smbios-discovery", "Checksum for manager: " << m_uuid << " updated"
                                                                              << " from: "
                                                                              << static_cast<uint16_t>(checksum)
                                                                              << " to: "
                                                                              << static_cast<uint16_t>(m_checksum));
            return true;
        }
    }
    catch (const std::exception& ex) {
        log_debug("smbios-discovery", "Unable to read checksum: " << ex.what());
    }
    return false;
}
