/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file fpga.hpp
 * @brief FPGA attribute
 * */

#pragma once



#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/fpga_interface.hpp"
#include "agent-framework/module/model/attributes/fpga_reconfiguration_slot.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

class Fpga {
public:
    using FpgaInterfaces = Array<enums::FpgaInterface>;
    using Slots = Array<FpgaReconfigurationSlot>;


    /*!
     * @brief Get FPGA PCI bus number
     * @return PCI bus of FPGA
     */
    uint8_t get_bus() const {
        return m_bus;
    }


    /*!
     * @brief Set FPGA PCI bus number
     * @param bus FPGA PCI bus
     */
    void set_bus(uint8_t bus) {
        m_bus = bus;
    }


    /*!
     * @brief Get FPGA PCI device number
     * @return PCI device number of FPGA
     */
    uint8_t get_device() const {
        return m_device;
    }


    /*!
     * @brief Set FPGA PCI device number
     * @param device PCI device number of FPGA
     */
    void set_device(uint8_t device) {
        m_device = device;
    }


    /*!
     * @brief Get FPGA PCI function number
     * @return PCI function number of FPGA
     */
    uint8_t get_function() const {
        return m_func;
    }


    /*!
     * @brief Set FPGA PCI function number
     * @param func PCI function number of FPGA
     */
    void set_function(uint8_t func) {
        m_func = func;
    }


    /*!
     * @brief Get FPGA type
     *
     * @return FPGA type
     */
    const OptionalField<enums::FpgaType>& get_type() const {
        return m_type;
    }


    /*!
     * @brief Set FPGA type
     *
     * @param type FPGA type
     */
    void set_type(const OptionalField<enums::FpgaType>& type) {
        m_type = type;
    }


    /*!
     * @brief Get FPGA model
     *
     * @return FPGA model
     */
    const OptionalField<std::string>& get_model() const {
        return m_model;
    }


    /*!
     * @brief Set FPGA model
     *
     * @param model FPGA model
     */
    void set_model(const OptionalField<std::string>& model) {
        m_model = model;
    }


    /*!
     * @brief Get FPGA Firmware ID
     *
     * @return FPGA Firmware ID
     */
    const OptionalField<std::string>& get_fw_id() const {
        return m_fw_id;
    }


    /*!
     * @brief Set FPGA Firmware ID
     *
     * @param fw_id FPGA Firmware ID
     */
    void set_fw_id(const OptionalField<std::string>& fw_id) {
        m_fw_id = fw_id;
    }


    /*!
     * @brief Get FPGA Firmware Manufacturer
     *
     * @return FPGA Firmware Manufacturer
     */
    const OptionalField<std::string>& get_fw_manufacturer() const {
        return m_fw_manufacturer;
    }


    /*!
     * @brief Set FPGA Firmware Manufacturer
     *
     * @param fw_manufacturer FPGA Firmware Manufacturer
     */
    void set_fw_manufacturer(const OptionalField<std::string>& fw_manufacturer) {
        m_fw_manufacturer = fw_manufacturer;
    }


    /*!
     * @brief Get FPGA Firmware Version
     *
     * @return FPGA Firmware Version
     */
    const OptionalField<std::string>& get_fw_version() const {
        return m_fw_version;
    }


    /*!
     * @brief Set FPGA Firmware Version
     *
     * @param fw_version FPGA Firmware Version
     */
    void set_fw_version(const OptionalField<std::string>& fw_version) {
        m_fw_version = fw_version;
    }


    /*!
     * @brief Get FPGA Host Interface
     *
     * @return FPGA Host Interface
     */
    const OptionalField<enums::FpgaInterface>& get_host_interface() const {
        return m_host_interface;
    }


    /*!
     * @brief Set FPGA Host Interface
     *
     * @param host_interface FPGA Host Interface
     */
    void set_host_interface(const OptionalField<enums::FpgaInterface>& host_interface) {
        m_host_interface = host_interface;
    }


    /*!
     * @brief Get FPGA External Interfaces Collection
     *
     * @return FPGA External Interfaces Collection
     */
    const FpgaInterfaces& get_external_interfaces() const {
        return m_external_interfaces;
    }


    /*!
     * @brief Set FPGA External Interfaces Collection
     *
     * @param external_interfaces FPGA External Interfaces Collection
     */
    void set_external_interfaces(const FpgaInterfaces& external_interfaces) {
        m_external_interfaces = external_interfaces;
    }


    /*!
     * @brief Get FPGA Sideband Interface
     *
     * @return FPGA Sideband Interface
     */
    const OptionalField<enums::FpgaInterface>& get_sideband_interface() const {
        return m_sideband_interface;
    }


    /*!
     * @brief Set FPGA Sideband Interface
     *
     * @param sideband_interface FPGA Sideband Interface
     */
    void set_sideband_interface(const OptionalField<enums::FpgaInterface>& sideband_interface) {
        m_sideband_interface = sideband_interface;
    }


    /*!
     * @brief Get FPGA PCIe Virtual Functions
     *
     * @return FPGA PCIe Virtual Functions
     */
    const OptionalField<std::uint32_t>& get_pcie_virtual_functions() const {
        return m_pcie_virtual_functions;
    }


    /*!
     * @brief Set FPGA PCIe Virtual Functions
     *
     * @param pcie_virtual_functions FPGA PCIe Virtual Functions
     */
    void set_pcie_virtual_functions(const OptionalField<std::uint32_t>& pcie_virtual_functions) {
        m_pcie_virtual_functions = pcie_virtual_functions;
    }


    /*!
     * @brief Get FPGA Reconfiguration Slots Details
     *
     * @return FPGA Reconfiguration Slots Details
     */
    const Array<FpgaReconfigurationSlot>& get_reconfiguration_slots_details() const {
        return m_reconfiguration_slots_details;
    }


    /*!
     * @brief Set FPGA slots
     *
     * @param slots FPGA slots
     */
    void set_reconfiguration_slots_details(const Array<FpgaReconfigurationSlot>& reconfiguration_slots_details) {
        m_reconfiguration_slots_details = reconfiguration_slots_details;
    }


    /*!
     * @brief Get FPGA programmable from host
     *
     * @return FPGA programmable from host
     */
    const OptionalField<bool>& get_programmable_from_host() const {
        return m_programmable_from_host;
    }


    /*!
     * @brief Set FPGA programmable from host
     *
     * @param programmable_from_host FPGA programmable from host
     */
    void set_programmable_from_host(const OptionalField<bool>& programmable_from_host) {
        m_programmable_from_host = programmable_from_host;
    }


    /*!
     * @brief Set whether the FPGA has been erased
     * @param erased FPGA's erasure status
     */
    void set_erased(bool erased) {
        m_erased = erased;
    }


    /*!
     * @brief Get erased
     * @return true if the FPGA has been erased
     */
    bool get_erased() const {
        return m_erased;
    }


    /*!
     * @brief construct an object of class Fpga from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Fpga object
     */
    static Fpga from_json(const json::Json& json);


    /*!
     * @brief Transform the object to JSon
     *
     * @return An Fpga object serialized to json::Json
     */
    json::Json to_json() const;


private:
    uint8_t m_bus{};
    uint8_t m_device{};
    uint8_t m_func{};
    OptionalField<enums::FpgaType> m_type{};
    OptionalField<std::string> m_model{};
    OptionalField<std::string> m_fw_id{};
    OptionalField<std::string> m_fw_manufacturer{};
    OptionalField<std::string> m_fw_version{};
    OptionalField<enums::FpgaInterface> m_host_interface{};
    FpgaInterfaces m_external_interfaces{};
    OptionalField<enums::FpgaInterface> m_sideband_interface{};
    OptionalField<std::uint32_t> m_pcie_virtual_functions{};
    OptionalField<bool> m_programmable_from_host{};
    Array<FpgaReconfigurationSlot> m_reconfiguration_slots_details{};
    bool m_erased{false};
};

}
}
}
