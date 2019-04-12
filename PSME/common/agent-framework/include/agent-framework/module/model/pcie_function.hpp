/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file pcie_function.hpp
 * @brief Definition of PCIe function class
 * */
#pragma once
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/pnc.hpp"

namespace agent_framework {
namespace model {

/*! @brief PCIe function class */
class PcieFunction : public Resource {
public:
    explicit PcieFunction(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::PcieDevice);
    ~PcieFunction();

    /*! Enable copy */
    PcieFunction(const PcieFunction&) = default;
    PcieFunction& operator=(const PcieFunction&) = default;
    /*! Enable move */
    PcieFunction(PcieFunction&&) = default;
    PcieFunction& operator=(PcieFunction&&) = default;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return PcieFunction::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return PcieFunction::component;
    }

    /*!
     * @brief Returns JSON representation of the object.
     * @return Json representation.
     * */
    json::Json to_json() const;

    /*!
     * @brief Constructs object from JSON
     * @param[in] json the json::Json deserialized to object
     * @return the newly constructed object
     */
    static PcieFunction from_json(const json::Json& json);

    /*!
     * @brief Set FunctionId
     * @param[in] function_id FunctionId
     * */
    void set_function_id(const OptionalField<std::string>& function_id) {
        m_function_id = function_id;
    }

    /*!
     * @brief Set Function type
     * @param[in] function_type Function type
     * */
    void set_function_type(const OptionalField<enums::PcieFunctionType>& function_type) {
        m_function_type = function_type;
    }

    /*!
     * @brief Set device class
     * @param[in] device_class Device class
     * */
    void set_device_class(const OptionalField<enums::PcieFunctionDeviceClass>& device_class) {
        m_device_class = device_class;
    }

    /*!
     * @brief Set PCI Device Id
     * @param[in] pci_device_id PCI Device Id
     * */
    void set_pci_device_id(const OptionalField<std::string>& pci_device_id) {
        m_pci_device_id = pci_device_id;
    }

    /*!
     * @brief Set PCI Vendor Id
     * @param[in] pci_vendor_id PCI Vendor Id
     * */
    void set_pci_vendor_id(const OptionalField<std::string>& pci_vendor_id) {
        m_pci_vendor_id = pci_vendor_id;
    }

    /*!
     * @brief Set PCI Class Code
     * @param[in] pci_class_code PCI Class Code
     * */
    void set_pci_class_code(const OptionalField<std::string>& pci_class_code) {
        m_pci_class_code = pci_class_code;
    }

    /*!
     * @brief Set PCI Revision Id
     * @param[in] pci_revision_id PCI Revision Id
     * */
    void set_pci_revision_id(const OptionalField<std::string>& pci_revision_id) {
        m_pci_revision_id = pci_revision_id;
    }

    /*!
     * @brief Set PCI Subsystem Id
     * @param[in] pci_subsystem_id PCI Subsystem Id
     * */
    void set_pci_subsystem_id(const OptionalField<std::string>& pci_subsystem_id) {
        m_pci_subsystem_id = pci_subsystem_id;
    }

    /*!
     * @brief Set PCI Subsystem Vendor Id
     * @param[in] pci_subsystem_vendor_id PCI Subsystem Vendor Id
     * */
    void set_pci_subsystem_vendor_id(const OptionalField<std::string>& pci_subsystem_vendor_id) {
        m_pci_subsystem_vendor_id = pci_subsystem_vendor_id;
    }

    /*!
     * @brief Set functional device
     * @param[in] functional_device Functional device
     * */
    void set_functional_device(const OptionalField<std::string>& functional_device) {
        m_functional_device = functional_device;
    }

    /*!
     * @brief Get Function Id
     * @return Function Id
     * */
    const OptionalField<std::string>& get_function_id() const {
        return m_function_id;
    }

    /*!
     * @brief Get Function Type
     * @return Function Code
     * */
    const OptionalField<enums::PcieFunctionType>& get_function_type() const {
        return m_function_type;
    }

    /*!
     * @brief Get Device Class
     * @return Device Class
     * */
    const OptionalField<enums::PcieFunctionDeviceClass>& get_device_class() const {
        return m_device_class;
    }

    /*!
     * @brief Get PCI Device Id
     * @return PCI Device
     * */
    const OptionalField<std::string>& get_pci_device_id() const {
        return m_pci_device_id;
    }

    /*!
     * @brief Get PCI Vendor Id
     * @return PCI Vendor
     * */
    const OptionalField<std::string>& get_pci_vendor_id() const {
        return m_pci_vendor_id;
    }

    /*!
     * @brief Get PCI Class Code
     * @return Class Code
     * */
    const OptionalField<std::string>& get_pci_class_code() const {
        return m_pci_class_code;
    }

    /*!
     * @brief Get PCI Revision Id
     * @return PCI Revision Id
     * */
    const OptionalField<std::string>& get_pci_revision_id() const {
        return m_pci_revision_id;
    }

    /*!
     * @brief Get PCI Subsystem Id
     * @return PCI Subsystem Id
     * */
    const OptionalField<std::string>& get_pci_subsystem_id() const {
        return m_pci_subsystem_id;
    }

    /*!
     * @brief Get PCI Subsystem Vendor Id
     * @return PCI Subsystem Vendor Id
     * */
    const OptionalField<std::string>& get_pci_subsystem_vendor_id() const {
        return m_pci_subsystem_vendor_id;
    }

    /*!
     * @brief Get Functional Device
     * @return Functional Device
     * */
    const OptionalField<std::string>& get_functional_device() const {
        return m_functional_device;
    }

    /*!
     * @brief Get downstream port uuid
     * @return downstream port uuid
     * */
    const std::string& get_dsp_port_uuid() const {
        return m_dsp_port_uuid;
    }

    /*!
     * @brief Set downstream port uuid
     * @param[in] dsp_port_uuid Downstream port uuid
     * */
    void set_dsp_port_uuid(const std::string& dsp_port_uuid) {
        m_dsp_port_uuid = dsp_port_uuid;
    }

private:
    OptionalField<std::string> m_function_id{};
    OptionalField<enums::PcieFunctionType> m_function_type{};
    OptionalField<enums::PcieFunctionDeviceClass> m_device_class{};
    OptionalField<std::string> m_pci_device_id{};
    OptionalField<std::string> m_pci_vendor_id{};
    OptionalField<std::string> m_pci_class_code{};
    OptionalField<std::string> m_pci_revision_id{};
    OptionalField<std::string> m_pci_subsystem_id{};
    OptionalField<std::string> m_pci_subsystem_vendor_id{};
    OptionalField<std::string> m_functional_device{};

    // Uuid of the downstream port of the function
    std::string m_dsp_port_uuid{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
