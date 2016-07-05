/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file submodule.hpp
 * @brief Submodule
 * */

#ifndef AGENT_FRAMEWORK_MODULE_SUBMODULE_HPP
#define AGENT_FRAMEWORK_MODULE_SUBMODULE_HPP

#include "agent-framework/module/storage_controller.hpp"
#include "agent-framework/module/network_interface.hpp"
#include "agent-framework/module/logical_drive.hpp"
#include "agent-framework/module/target_manager.hpp"
#include "agent-framework/module/iscsi_data.hpp"
#include "agent-framework/module/vlan.hpp"
#include "agent-framework/module/port.hpp"
#include "agent-framework/module/vlanport.hpp"

#include "agent-framework/module/generic_manager.hpp"
#include "agent-framework/module/processor.hpp"
#include "agent-framework/module/memory.hpp"

#include "uuid++.hh"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#include <string>
#include <vector>
#include <algorithm>

namespace json { class Value; }

namespace agent_framework {
namespace generic {

using NetworkInterfaceUniquePtr = NetworkInterface::NetworkInterfaceUniquePtr;
using LogicalDriveSharedPtr = LogicalDrive::LogicalDriveSharedPtr;
using LogicalDriveWeakPtr = LogicalDrive::LogicalDriveWeakPtr;
using TargetSharedPtr = Target::TargetSharedPtr;
using VlanPortUniquePtr = VlanPort::VlanPortUniquePtr;
using PortSharedPtr = Port::PortSharedPtr;
using VlanSharedPtr = Vlan::VlanSharedPtr;
using PublicVlans = std::vector<std::uint32_t>;

using ProcessorManager = GenericManager<Processor>;
using MemoryManager = GenericManager<Memory>;

/*! Submodule class */
class Submodule {
    const std::string BLADE_TYPE = "Blade";

    uuid m_name{};
    std::string m_type{};
    std::string m_ip_address{};
    uint32_t m_port{};
    uint32_t m_configured_port{};
    std::string m_username{};
    std::string m_password{};
    bool m_presence{false};
    IscsiData m_iscsi_data{};
    std::uint32_t m_slot{};
    PublicVlans m_public_vlans{};

    std::vector<StorageControllerUniquePtr> m_storage_controllers{};
    std::vector<NetworkInterfaceUniquePtr> m_network_interfaces{};
    std::vector<LogicalDriveSharedPtr> m_logical_drives{};
    std::vector<VlanPortUniquePtr> m_vlanports{};
    std::vector<VlanSharedPtr> m_vlans{};
    std::vector<PortSharedPtr> m_ports{};

    TargetManager m_target_manager{};
    ProcessorManager m_processor_manager{};
    MemoryManager m_memory_manager{};

public:
    /* Submodule unique pointer */
    using SubmoduleUniquePtr = std::unique_ptr<Submodule>;

    /*! Default constructor */
    Submodule() {
        m_name.make(UUID_MAKE_V1);
        m_type = BLADE_TYPE;
    }

    /*! Creates Subodule unique object */
    static SubmoduleUniquePtr make_submodule() {
        return SubmoduleUniquePtr{ new Submodule() };
    }

    /*!
     * @brief Gets name of submodule.
     * @return Name of submodule represented by UUID.
     * */
    const std::string get_name() { return std::string(m_name.string()); }

    /*!
     * @brief Sets type of submodule.
     *
     * @param[in] type Type to set.
     * */
    void set_type(std::string& type) { m_type = type; }

    /*!
     * @brief Gets type of submodule.
     * @return Type of submodule.
     * */
    std::string get_type() const { return m_type; }

    /*!
     * @brief Sets IP address of submodule.
     *
     * @param[in] ip_address IP address.
     * */
    void set_ip_address(const std::string& ip_address) {
        m_ip_address = ip_address;
    }

    /*!
     * @brief Gets IP address of submodule.
     * @return IP address of submodule.
     * */
    const std::string& get_ip_address() const { return m_ip_address; }

    /*!
     * @brief Sets username.
     *
     * @param[in] username User name.
     * */
    void set_username(const std::string& username) { m_username = username; }

    /*!
     * @brief Gets user name.
     * @return User name.
     * */
    const std::string& get_username() const { return m_username; }

    /*!
     * @brief Sets password.
     *
     * @param[in] password Password.
     * */
    void set_password(const std::string& password) { m_password = password; }

    /*!
     * @brief Gets password.
     * @return Password.
     * */
    const std::string& get_password() const { return m_password; }

    /*!
     * @brief Sets port.
     *
     * @param[in] port Port.
     * */
    void set_port(uint32_t port) { m_port = port; }

    /*!
     * @brief Gets port.
     * @return Port.
     * */
    uint32_t get_port() const { return m_port; }

    /*!
     * @brief Sets port to value read from configuration.
     * */
    void reset_port() { m_port = m_configured_port; }

    /*!
     * @brief Load settings from configuration
     *
     * @param[in] submod_configuration Submodule configuration section
     * */
    void read_configuration(const json::Value& submod_configuration);

    /*!
     * @brief Sets presence.
     * @param presence of the module.
     */
    void set_presence(const bool presence) {
        m_presence = presence;
    }

    /*!
     * @brief Gets presence of the Submodule.
     * @return true if Submodule is present, otherwise false.
     */
    bool is_present() const {
        return m_presence;
    }

    /*!
     * @brief Adds new submodule to module.
     *
     * @param[in] controller  Pointer to new storage controller.
     * */
    void add_storage_controller(StorageControllerUniquePtr controller) {
        m_storage_controllers.push_back(std::move(controller));
    }

    /*!
     * @brief Returns vector of submodules.
     *
     * @return Reference to vector of submodules.
     * */
    const std::vector<StorageControllerUniquePtr>& get_storage_controllers() const {
        return m_storage_controllers;
    }

    /*!
     * @brief Returns vector of hard drives.
     * Note, that this method returns all hard drives from
     * all storage controllers of submodule.
     *
     * @return Vector of hard drives.
     * */
    std::vector<HardDriveSharedPtr> get_hard_drives() const;

    /*!
     * @brief Adds new submodule to module.
     *
     * @param[in] logical_drive Pointer to new logical_drive.
     * */
    void add_logical_drive(LogicalDriveSharedPtr logical_drive) {
        m_logical_drives.push_back(logical_drive);
    }

    /*!
     * @brief Returns all logical drives under this submodule.
     *
     * @return Vector of all logical drives under this submodule.
     * */
    const std::vector<LogicalDriveSharedPtr> get_logical_drives() const;

    /*!
     * @brief Returns vector of targets.
     *
     * @return Reference to vector of targets.
     * */
    const std::vector<TargetSharedPtr>& get_targets() const {
        return get_target_manager().get_targets();
    }

    /*!
     * @brief Adds new network interface to submodule.
     *
     * @param[in] network_interface  Pointer to new network interface.
     * */
    void add_network_interface(NetworkInterfaceUniquePtr network_interface) {
        m_network_interfaces.push_back(std::move(network_interface));
    }

    /*!
     * @brief Returns vector of network interfaces.
     *
     * @return Reference to vector of network interfaces.
     * */
    const std::vector<NetworkInterfaceUniquePtr>& get_network_interfaces() const {
        return m_network_interfaces;
    }

    /*!
     * @brief Returns target manager
     * @return Target manager const reference
     * */
    const TargetManager& get_target_manager() const {
        return m_target_manager;
    }

    /*!
     * @brief Returns target manager
     * @return Target manager reference
     * */
    TargetManager& get_target_manager() {
        return  const_cast<TargetManager&>(
                static_cast<const Submodule*>(this)-> get_target_manager());
    }

    /*!
     * @brief Returns processor manager
     * @return Processor manager const reference
     * */
    const ProcessorManager& get_processor_manager() const {
        return m_processor_manager;
    }

    /*!
     * @brief Returns target manager
     * @return Target manager reference
     * */
    ProcessorManager& get_processor_manager() {
        return  const_cast<ProcessorManager&>(
                static_cast<const Submodule*>(this)-> get_processor_manager());
    }

    /*!
     * @brief Returns memory manager
     * @return memory manager const reference
     * */
    const MemoryManager& get_memory_manager() const {
        return m_memory_manager;
    }

    /*!
     * @brief Returns memory manager
     * @return Memory manager reference
     * */
    MemoryManager& get_memory_manager() {
        return  const_cast<MemoryManager&>(
                static_cast<const Submodule*>(this)-> get_memory_manager());
    }

    /*!
     * @brief Adds new Vlan to switch/submodule.
     *
     * @param[in] vlan Pointer to new vlan.
     * */
    void add_vlan(VlanSharedPtr vlan) {
        m_vlans.push_back(std::move(vlan));
    }

    /*!
     * @brief Returns vector of vlans
     *
     * @return Reference to vector of vlans.
     * */
    const std::vector<VlanSharedPtr>& get_vlans() const {
        return m_vlans;
    }

    /*!
     * @brief Adds new port to Switch.
     * @param[in] port Pointer to new port.
     * */
    void add_switch_port(PortSharedPtr port) {
        m_ports.push_back(std::move(port));
    }

    /*!
     * @brief Returns vector of ports.
     * @return Reference to vector of ports.
     * */
    const std::vector<PortSharedPtr>& get_switch_ports() const {
        return m_ports;
    }

    /*!
     * @brief Adds new Vlanport to Switch.
     * @param[in] vlanport Pointer to new VlanPort.
     * */
    void add_vlanport(VlanPortUniquePtr vlanport) {
        m_vlanports.push_back(std::move(vlanport));
    }

    /*!
     * @brief Returns vector of VlanPorts.
     * @return Reference to vector of VlanPorts.
     * */
    const std::vector<VlanPortUniquePtr>& get_vlanports() const {
        return m_vlanports;
    }

    /*!
     * @brief Set iscsi data object
     * @param iscsi_data IscsiData object
     */
    void set_iscsi_data(const IscsiData& iscsi_data) {
        m_iscsi_data = iscsi_data;
    }

    /*!
     * @brief Get iscsi data object
     * @return IscsiData object
     */
    const IscsiData& get_iscsi_data() const {
        return m_iscsi_data;
    }

    /*!
     * @brief Set slot
     * @param slot Slot
     */
    void set_slot(const std::uint32_t slot) {
        m_slot = slot;
    }

    /*!
     * @brief Get slot
     * @return Slot
     */
    std::uint32_t get_slot() const {
        return m_slot;
    }

    /*!
     * @brief Set public VLANs
     * @param public_vlans Public VLANs
     */
    void set_public_vlan_id(const PublicVlans& public_vlans) {
        m_public_vlans = public_vlans;
    }

    /*!
     * @brief Get public VLANs
     * @return public VLANs
     */
    PublicVlans get_public_vlans() const {
        return m_public_vlans;
    }

    /*!
     * @brief Find hard drive with given UUID
     * @param[in] uuid Hard drive UUID
     * @return Hard drive with given UUID
     * */
    HardDriveWeakPtr find_hard_drive(const string& uuid) const;

    /*!
     * @brief Find logical drive with given UUID
     * @param[in] uuid Logical drive UUID
     * @return Logical drive with given UUID
     * */
    LogicalDriveWeakPtr find_logical_drive(const string& uuid) const;

    /*!
     * @brief Find target with given UUID
     * @param[in] uuid target UUID
     * @return Target with given UUID
     * */
    Target::TargetWeakPtr find_target(const string& uuid) const;

    /*! Default destrcutor */
    ~Submodule();

private:
    void m_read_storage_controllers_configuration(
        const json::Value& controllers_configuration);

    void m_read_network_interfaces_configuration(
        const json::Value& interfaces_configuration);

    void m_read_iscsi_configuration(
        const json::Value& iscsi_configuration);

    void m_read_vlans(
            const json::Value& vlans_configuration);

    void m_read_public_vlans(
            const json::Value& public_vlans_configuration);

    void m_read_ports(
            const json::Value& ports_configuration);

    void m_read_vlanports(
            const json::Value& vlans_configuration);

    const std::string m_get_interface_ip(const std::string& interface);

    VlanSharedPtr m_get_vlan_by_id(const uint32_t vlan_id);

    PortSharedPtr m_get_port_by_id(const uint32_t port_id);
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_SUBMODULE_HPP */
