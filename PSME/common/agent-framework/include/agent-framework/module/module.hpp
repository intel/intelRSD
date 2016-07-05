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
 * @file module.hpp
 * @brief Module
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODULE_HPP
#define AGENT_FRAMEWORK_MODULE_MODULE_HPP

#include "agent-framework/status/module_software_status.hpp"
#include "agent-framework/status/module_hardware_status.hpp"
#include "agent-framework/module/submodule.hpp"
#include "agent-framework/module/target.hpp"

#include "uuid++.hh"

#include <vector>
#include <string>

using std::string;

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

using SubmoduleUniquePtr = Submodule::SubmoduleUniquePtr;

/*! Module class */
class Module {
    const std::string COMPUTE_MODULE_TYPE = "ComputeModule";

    uuid m_name{};
    std::string m_type{};
    std::string m_ip_address{};
    uint32_t m_port{};
    std::string m_username{};
    std::string m_password{};
    std::string m_product_name{};
    std::string m_manufacturer_name{};
    std::uint32_t m_slot{};
    bool m_presence{false};

    std::vector<SubmoduleUniquePtr> m_submodules{};

public:
    /*! Module unique pointer */
    using ModuleUniquePtr = std::unique_ptr<Module>;

    /*!
     * @brief Default constructor
     * */
    Module(const std::string ip_address = "1.1.2.1") {
        m_name.make(UUID_MAKE_V1);
        m_ip_address = ip_address;
        m_type = COMPUTE_MODULE_TYPE;
    }

    /*! Creates Module unique object */
    static ModuleUniquePtr make_module(const std::string& ipv4) {
        return ModuleUniquePtr{ new Module(ipv4) };
    }

    /*!
     * @brief Gets name of Module.
     * @return Name of Module represented by UUID.
     * */
    const std::string get_name() {
        return std::string(m_name.string());
    }

    /*!
     * @brief Gets IP address of Module
     * @return IP address of module
     * */
    const std::string& get_ip_address() const {
        return m_ip_address;
    }

    /*!
     * @brief Gets user name
     * @return User name
     * */
    const std::string& get_username() const {
        return m_username;
    }

    /*!
     * @brief Gets password
     * @return Password
     * */
    const std::string& get_password() const {
        return m_password;
    }

    /*!
     * @brief Gets port
     * @return Port
     * */
    uint32_t get_port() const {
        return m_port;
    }

    /*!
     * @brief Gets type of Module.
     * @return Type of Module.
     * */
    const std::string get_type() const {
        return m_type;
    }

    /*!
     * @brief Sets human readable product name.
     * @param name string with product name.
     */
    void set_product_name(const string& name) {
        m_product_name = name;
    }

    /*!
     * @brief Sets human readable manufacturer name.
     * @param name string with manufacturer name.
     */
    void set_manufacturer_name(const string& name) {
        m_product_name = name;
    }

    /*!
     * @brief Gets human readable product name.
     * @return string with product name.
     */
    const string& get_product_name() const {
        return m_product_name;
    }

    /*!
     * @brief Gets human readable manufacturer name.
     * @return string with manufacturer name.
     */
    const string& get_manufacturer_name() const {
        return m_manufacturer_name;
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
     * @brief Load settings from configuration
     *
     * @param[in] module_configuration Module configuration section
     * */
    void read_configuration(const json::Value& module_configuration);

    /*!
     * @brief Adds new submodule to module.
     *
     * @param[in] submodule Pointer to new submodule.
     * */
    void add_submodule(SubmoduleUniquePtr submodule);

    /*!
     * @brief Returns vector of submodules.
     *
     * @return Reference to vector of submodules.
     * */
    const std::vector<SubmoduleUniquePtr>& get_submodules() const {
        return m_submodules;
    }

    /*!
     * @brief Gets raw pointer to Submodule to edit data.
     * @param number which submodule will be returned.
     * @return raw pointer to Submodule.
     */
    const SubmoduleUniquePtr& get_submodule(unsigned long int number) const {
        return m_submodules.at(number);
    }

    /*!
     * @brief Performs resize operation on encapsulated Submodule vector.
     * @param new_size new size of the Submodule vector.
     */
    void resize_submodules_vector(unsigned long int new_size) {
        m_submodules.resize(new_size);
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

    /*!
     * @brief Returns vector of hard drives.
     * Note, that this method returns all hard drives from
     * all submodules of module.
     *
     * @return Vector of hard drives.
     * */
    std::vector<HardDriveSharedPtr> get_hard_drives() const;
};

}
}
#endif /* AGENT_FRAMEWORK_MODULE_MODULE_HPP */
