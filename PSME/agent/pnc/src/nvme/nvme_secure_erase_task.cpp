/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file nvme_secure_erase_task.cpp
 *
 * @brief NVMEe secure erase task implementation
 * */


#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/partition_binding_info.hpp"
#include "nvme/nvme_secure_erase_task.hpp"
#include "nvme/nvme_secure_erase.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "tools/toolset.hpp"



using namespace agent_framework::model;
using namespace agent_framework::eventing;
using namespace agent_framework::module;
using namespace agent::pnc::nvme;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::tools;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;

namespace {

void set_drive_erased(std::string drive_uuid) {
    get_manager<Drive>().get_entry_reference(drive_uuid)->set_erased(true);
}

const std::vector<std::string> get_devices(const Toolset& tools, const GlobalAddressSpaceRegisters& gas,
        const Switch& sw, const std::string& drive_uuid) {

    auto ports = tools.model_tool->get_ports_by_drive_uuid(drive_uuid);
    if (ports.size() == 0) {
        throw std::runtime_error(std::string{"No functions on drive"});
    }
    auto port_uuid = ports.front();
    PortBindingInfo pbi = tools.gas_tool->get_port_binding_info(gas,
        uint8_t(get_manager<Port>().get_entry(port_uuid).get_phys_port_id()));

    auto logical_bridge_id = pbi.output.fields.port_binding_info->logical_bridge_id;
    SysfsReader reader{};
    std::vector<std::string> drive_names = reader.get_drives_for_device(sw.get_bridge_path(),
                                                                        sw.get_sec_bus_num(),
                                                                        uint8_t(logical_bridge_id - 1));
    return drive_names;
}

}


NvmeSecureEraseTask::NvmeSecureEraseTask(const std::string& drive_uuid) :
    m_drive_uuid{drive_uuid} {}


void NvmeSecureEraseTask::operator()() {
    log_info(GET_LOGGER("agent"), "Erasing drive securely started.");

    Toolset tools = Toolset::get();

    Switch sw = get_manager<Switch>().get_entry(tools.model_tool->get_switch_for_drive_uuid(m_drive_uuid));
    auto gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());
    auto devices = get_devices(tools, gas, sw, m_drive_uuid);

    if (devices.size() > 0) {
        std::string device_path = devices.front();
        device_path = std::string("/dev/" + device_path);
        log_debug(GET_LOGGER("agent"), "Drive to be erased: " + device_path);
        try {
            secure_erase(device_path);
            set_drive_erased(m_drive_uuid);
            tools.gas_tool->unbind_drive_from_mgmt_partition(tools.model_tool, gas, m_drive_uuid);
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("agent"), "An error occured while erasing drive: " << e.what());
            tools.model_tool->set_drive_status(m_drive_uuid,
                attribute::Status{enums::State::StandbyOffline, enums::Health::Warning});
        }
        tools.model_tool->set_drive_is_being_erased(m_drive_uuid, false);
        log_info(GET_LOGGER("agent"), "Erasing drive securely finished.");
    }
    else {
        tools.model_tool->set_drive_status(m_drive_uuid,
            attribute::Status{enums::State::StandbyOffline, enums::Health::Warning});
        tools.model_tool->set_drive_is_in_warning_state(m_drive_uuid, true);
        tools.model_tool->set_drive_is_being_erased(m_drive_uuid, false);
        THROW(agent_framework::exceptions::PncError, "agent", "No NVMe devices found in the OS.");

    }
}
