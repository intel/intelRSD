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
 *
 * @file secure_erase_task.hpp
 * @brief Secure erase task
 * */

#pragma once



#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/partition_binding_info.hpp"
#include "nvme/nvme_secure_erase.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "tools/toolset.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "opaepp/opae/opae_cpp_device_updater.hpp"
#include "opaepp/opae/opae_cpp_device_reader.hpp"
#include "loader/pnc_configuration.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"

#include <string>
#include <fstream>



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::tools;
using namespace agent::pnc::gas::mrpc;

namespace {
/*!
 * @brief Get nvme devices from OS
 * @param tools tools
 * @param gas gas
 * @param sw switch
 * @param drive_uuid that is been searched
 * @return vector of strings - nvme devices in the OS
 */
const std::vector<std::string> get_devices(const Toolset& tools, const GlobalAddressSpaceRegisters& gas,
                                           const Switch& sw, const std::string& drive_uuid) {

    auto ports = tools.model_tool->get_ports_by_device_uuid(drive_uuid);
    if (ports.size() == 0) {
        throw std::runtime_error(std::string{"No functions on drive"});
    }
    auto port_uuid = ports.front();
    PortBindingInfo pbi = tools.gas_tool->get_port_binding_info(gas,
                                                                uint8_t(get_manager<Port>().get_entry(
                                                                    port_uuid).get_phys_port_id()));

    auto logical_bridge_id = pbi.output.fields.port_binding_info->logical_bridge_id;
    SysfsReader reader{};
    std::vector<std::string> drive_names = reader.get_drives_for_device(sw.get_bridge_path(),
                                                                        sw.get_sec_bus_num(),
                                                                        uint8_t(logical_bridge_id - 1));
    return drive_names;
}


PcieAddress get_pcie_address_for_fpga_device_uuid(const Toolset& tools, const GlobalAddressSpaceRegisters& gas,
                                                  const Switch& sw, const std::string& processor_uuid) {

    auto ports = tools.model_tool->get_ports_by_device_uuid(processor_uuid);
    if (ports.size() == 0) {
        throw std::runtime_error(std::string{"No functions on Processor"});
    }
    auto port_uuid = ports.front();
    PortBindingInfo pbi = tools.gas_tool->get_port_binding_info(gas,
                                                                uint8_t(get_manager<Port>().get_entry(
                                                                    port_uuid).get_phys_port_id()));

    auto logical_bridge_id = pbi.output.fields.port_binding_info->logical_bridge_id;

    SysfsReader reader{};

    PcieAddress sysfs_pcie_address = reader.get_pcie_address_for_fpga_device(sw.get_bridge_path(),
                                                                             sw.get_sec_bus_num(),
                                                                             uint8_t(logical_bridge_id - 1));
    return sysfs_pcie_address;
}

}

namespace agent {
namespace pnc {
namespace tools {

/*!
 * @brief Class for performing secure erase action on Drives and processors
 * @tparam Device Class representing Drive or FPGA Processor
 */
template<typename Device>
class PerformSecureErase {
public:
    /*!
     * @brief Default constructor
     */
    PerformSecureErase() = default;


    /*!
     * @brief Method for performing secure erase action
     */
    static void perform_secure_erase(const std::string& /*device_uuid*/);

};

/*!
 * @brief Specialization of PerformSecureErase for Drive, calls ioctl secure erase action
 */
template<>
class PerformSecureErase<Drive> {
public:
    PerformSecureErase() = default;


    static void perform_secure_erase(const std::string& m_device_uuid) {
        Toolset tools = Toolset::get();

        Switch sw = get_manager<Switch>().get_entry(tools.model_tool->get_switch_for_device_uuid(m_device_uuid));
        auto gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());
        auto devices = get_devices(tools, gas, sw, m_device_uuid);
        if (devices.size() > 0) {
            std::string device_path = devices.front();
            device_path = std::string("/dev/" + device_path);
            log_debug("agent", "Drive to be erased: " + device_path);
            nvme::secure_erase(device_path);
            tools.model_tool->update_device_erased<Drive>(m_device_uuid, true);
        }
        else {
            THROW(agent_framework::exceptions::PncError, "agent", "No NVMe devices found in the OS.");
        }
    };
};

/*!
 * @brief Specialization of PerformSecureErase for Processor, calls reprogram from OPAE
 */
template<>
class PerformSecureErase<Processor> {
public:
    PerformSecureErase() = default;


    static void perform_secure_erase(const std::string& device_uuid) {

        log_notice("agent", "Erasing processor");

        opaepp::OpaeCppDeviceUpater updater{};
        Toolset tools = Toolset::get();
        Switch sw = get_manager<Switch>().get_entry(tools.model_tool->get_switch_for_device_uuid(device_uuid));
        auto gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

        auto gbs_path = loader::PncConfiguration::get_instance()->get_secure_erase_gbs();
        std::vector<uint8_t> gbs;
        std::ifstream gbs_file(gbs_path, std::ios::binary);

        if (gbs_file) {
            gbs_file.unsetf(std::ios::skipws);
            gbs_file.seekg(0, std::ios::end);
            std::streampos file_size = gbs_file.tellg();
            gbs_file.seekg(0, std::ios::beg);

            gbs.reserve(size_t(file_size));

            gbs.insert(gbs.begin(),
                       std::istream_iterator<std::uint8_t>(gbs_file),
                       std::istream_iterator<std::uint8_t>());
            gbs_file.close();

            auto pcie_address = get_pcie_address_for_fpga_device_uuid(tools, gas, sw, device_uuid);

            /* Calls opae and performs the actual update */
            try {
                updater.update(pcie_address.bus, pcie_address.device, pcie_address.function, gbs.data(), gbs.size());
            }
            catch (std::exception& e) {
                std::string message = e.what();
                THROW(agent_framework::exceptions::PncError, "agent", message);
            }

        }
        else {
            THROW(agent_framework::exceptions::PncError, "agent", "GBS file provided in the config could not be read");
        }
    }
};

/*! @brief  Secure erase task class */
template<typename T>
class SecureEraseTask {
public:
    /*!
     * @brief Constructor
     * @param[in] device_uuid device uuid to be erased
     * */
    explicit SecureEraseTask(const std::string& device_uuid) :
        m_device_uuid{device_uuid} {}


    /*! @brief Starts erasing */
    void operator()() {
        log_info("agent", "Erasing device securely started.");

        Toolset tools = Toolset::get();

        Switch sw = get_manager<Switch>().get_entry(tools.model_tool->get_switch_for_device_uuid(m_device_uuid));
        auto gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

        try {
            PerformSecureErase<T>::perform_secure_erase(m_device_uuid);
            tools.model_tool->update_device_erased<T>(m_device_uuid, true);
            tools.gas_tool->unbind_device_from_mgmt_partition(tools.model_tool, gas, m_device_uuid);
        }
        catch (const std::runtime_error& e) {
            log_error("agent", "An error occurred while erasing: " << e.what());
            // throw task exception, device status will by updated by the callback
            THROW(agent_framework::exceptions::PncError, "agent", "Secure erase failed");
        }
        tools.model_tool->set_device_being_erased<T>(m_device_uuid, false);
        log_info("agent", "Erasing device securely finished.");
    }


private:
    std::string m_device_uuid{};


    const std::string& get_device_uuid() const {
        return m_device_uuid;
    }
};

}
}
}
