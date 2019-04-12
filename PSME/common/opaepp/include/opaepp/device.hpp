/*!
 * @brief OPAE device (FGPA or AFU) interface
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file device.hpp
 */

#pragma once



#include "agent-framework/module/utils/optional_field.hpp"
#include "opae/fpga.h"
#include <vector>



// needed to use OptionalField<fpga_version>
bool operator!=(const fpga_version& a, const fpga_version& b);


namespace opaepp {

class Device {
public:
    friend class OpaeProxyDeviceReader;


    /*!
     * @brief Get device type
     * @return device type
     * */
    fpga_objtype get_device_type() const {
        return m_device_type;
    }


    /*!
     * @brief Get device bus
     * @return device bus
     * */
    uint8_t get_bus() const {
        return m_bus;
    }


    /*!
     * @brief Get device
     * @return device
     * */
    uint8_t get_device() const {
        return m_device;
    }


    /*!
     * @brief Get function
     * @return function
     * */
    uint8_t get_function() const {
        return m_function;
    }


    /*!
     * @brief Get PCI address
     * @return return address in bus:device.function notation
     * */
    std::string get_pci_address() const;


    /*!
     * @brief Get PCI address for input bus, device, function parameters
     * @param bus PCI bus number
     * @param device PCI device number
     * @param function PCI function number
     * @return return address in bus:device.function notation
     * */
    static std::string get_pci_address(uint8_t bus, uint8_t device, uint8_t function);


    /*!
     * @brief Get socket id
     * @return socket id
     * */
    uint8_t get_socket_id() const {
        return m_socket_id;
    }


    /*!
     * @brief Get device id
     * @return device id
     * */
    const OptionalField<uint16_t>& get_device_id() const {
        return m_device_id;
    }


    /*!
     * @brief Get number of slots
     * @return number of slots
     * */
    const OptionalField<uint32_t>& get_num_slots() const {
        return m_num_slots;
    }


    /*!
     * @brief Get blue bitstream id
     * @return blue bitstream id
     * */
    const OptionalField<uint64_t>& get_blue_bitstream_id() const {
        return m_blue_bitstream_id;
    }


    /*!
     * @brief Get blue bitstream version
     * @return blue bitstream version
     * */
    const OptionalField<fpga_version>& get_blue_bitstream_version() const {
        return m_blue_bitstream_version;
    }


    /*!
     * @brief Get vendor id
     * @return vendor id
     * */
    const OptionalField<uint16_t>& get_vendor_id() const {
        return m_vendor_id;
    }


    /*!
     * @brief Get device model
     * @return device model
     * */
    const OptionalField<std::string>& get_model() const {
        return m_model;
    }


    /*!
     * @brief Get local memory size
     * @return local memory size
     * */
    const OptionalField<uint64_t>& get_local_memory_size() const {
        return m_local_memory_size;
    }


    /*!
     * @brief Get device capabilities
     * @return device capabilities
     * */
    const OptionalField<uint64_t>& get_capabilities() const {
        return m_capabilities;
    }


    /*!
     * @brief Get device GUID
     * @return device GUID
     * */
    const fpga_guid& get_guid() const {
        return m_guid;
    }


    /*!
     * @brief Get MMIO spaces
     * @return MMIO spaces
     * */
    const OptionalField<uint32_t>& get_mmio_spaces() const {
        return m_mmio_spaces;
    }


    /*!
     * @brief Get number of interrupts
     * @return number of interrupts
     * */
    const OptionalField<uint32_t>& get_num_interrupts() const {
        return m_num_interrupts;
    }


    /*!
     * @brief Get Accelerator state
     * @return Accelerator state
     * */
    const OptionalField<fpga_accelerator_state>& get_accelerator_state() const {
        return m_accelerator_state;
    }


    /*!
     * @brief Get object id
     * @return object id
     * */
    uint64_t get_object_id() const {
        return m_object_id;
    }


    /*!
     * @brief Set device type
     * @param[in] device_type device type
     */
    void set_device_type(const fpga_objtype device_type) {
        m_device_type = device_type;
    }


    /*!
     * @brief Set bus
     * @param[in] bus bus
     */
    void set_bus(uint8_t bus) {
        m_bus = bus;
    }


    /*!
     * @brief Set device
     * @param[in] device device
     */
    void set_device(uint8_t device) {
        m_device = device;
    }


    /*!
     * @brief Set function
     * @param[in] function function
     */
    void set_function(uint8_t function) {
        m_function = function;
    }


    /*!
     * @brief Set socket id
     * @param[in] socket_id socket id
     */
    void set_socket_id(uint8_t socket_id) {
        m_socket_id = socket_id;
    }


    /*!
     * @brief Set device id
     * @param[in] device_id device id
     */
    void set_device_id(const OptionalField<uint16_t>& device_id) {
        m_device_id = device_id;
    }


    /*!
     * @brief Set guid
     * @param[in] guid_raw_ptr pointer to raw guid object
     */
    void set_guid(const uint8_t* guid_raw_ptr) {
        std::copy_n(guid_raw_ptr, GUID_LENGTH, m_guid);
    }


    /*!
     * @brief Set object id
     * @param[in] object_id object id
     */
    void set_object_id(uint64_t object_id) {
        m_object_id = object_id;
    }


    /*!
     * @brief Set number of slots
     * @param[in] num_slots number of slots
     */
    void set_num_slots(const OptionalField<uint32_t>& num_slots) {
        m_num_slots = num_slots;
    }


    /*!
     * @brief Set blue bit stream id
     * @param[in] blue_bitstream_id id of blue bit stream
     */
    void set_blue_bitstream_id(const OptionalField<uint64_t>& blue_bitstream_id) {
        m_blue_bitstream_id = blue_bitstream_id;
    }


    /*!
     * @brief Set version of blue bit stream
     * @param[in] blue_bitstream_version version of blue bit stream
     */
    void set_blue_bitstream_version(const OptionalField<fpga_version>& blue_bitstream_version) {
        m_blue_bitstream_version = blue_bitstream_version;
    }


    /*!
     * @brief Set vendor id
     * @param[in] vendor_id vendor id
     */
    void set_vendor_id(const OptionalField<uint16_t>& vendor_id) {
        m_vendor_id = vendor_id;
    }


    /*!
     * @brief Set model
     * @param[in] model model
     */
    void set_model(const OptionalField<std::string>& model) {
        m_model = model;
    }


    /*!
     * @brief Set local memory size
     * @param[in] local_memory_size local memory size
     */
    void set_local_memory_size(const OptionalField<uint64_t>& local_memory_size) {
        m_local_memory_size = local_memory_size;
    }


    /*!
     * @brief Set capabilities
     * @param[in] capabilities capabilities
     */
    void set_capabilities(const OptionalField<uint64_t>& capabilities) {
        m_capabilities = capabilities;
    }


    /*!
     * @brief Set mmio spaces
     * @param[in] mmio_spaces mmio spaces
     */
    void set_mmio_spaces(const OptionalField<uint32_t>& mmio_spaces) {
        m_mmio_spaces = mmio_spaces;
    }


    /*!
     * @brief Set number of interrupts
     * @param[in] num_interrupts number of interrupts
     */
    void set_num_interrupts(const OptionalField<uint32_t>& num_interrupts) {
        m_num_interrupts = num_interrupts;
    }


    /*!
     * @brief Set accelerator state
     * @param[in] accelerator_state accelerator state
     */
    void set_accelerator_state(const OptionalField<fpga_accelerator_state>& accelerator_state) {
        m_accelerator_state = accelerator_state;
    }


private:
    static constexpr const size_t GUID_LENGTH = 16;

    fpga_objtype m_device_type{};

    /* PCI bus, device and function */
    uint8_t m_bus{};
    uint8_t m_device{};
    uint8_t m_function{};

    uint8_t m_socket_id{};
    OptionalField<uint16_t> m_device_id{};
    fpga_guid m_guid{};
    uint64_t m_object_id{};

    // fpga-only fields:
    OptionalField<uint32_t> m_num_slots{};
    OptionalField<uint64_t> m_blue_bitstream_id{};
    OptionalField<fpga_version> m_blue_bitstream_version{};
    OptionalField<uint16_t> m_vendor_id{}; /* not supported in OPAE. */
    OptionalField<std::string> m_model{}; /* not implemented in OPAE */
    OptionalField<uint64_t> m_local_memory_size{}; /* not implemented in OPAE */
    OptionalField<uint64_t> m_capabilities{}; /* Bitfield. Not implemented in OPAE. */

    // accelerator-only fields:
    OptionalField<uint32_t> m_mmio_spaces{};
    OptionalField<uint32_t> m_num_interrupts{};
    OptionalField<fpga_accelerator_state> m_accelerator_state{};


    friend std::ostream& operator<<(std::ostream& os, const Device& device);
};


std::ostream& operator<<(std::ostream& os, const Device& device);


std::ostream& operator<<(std::ostream& os, const std::vector<Device>& devices);
}
