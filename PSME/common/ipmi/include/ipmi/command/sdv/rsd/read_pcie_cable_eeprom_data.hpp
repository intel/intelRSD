/*!
 * @brief ReadPcieCableEepromData command interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file read_pcie_cable_eeprom_data.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"

#include <vector>
#include <cstdint>



namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief ReadPcieCableEepromData request.
 */
class ReadPcieCableEepromData : public Request {
public:
    ReadPcieCableEepromData();


    ReadPcieCableEepromData(const ReadPcieCableEepromData&) = default;


    ReadPcieCableEepromData(ReadPcieCableEepromData&&) = default;


    ReadPcieCableEepromData& operator=(const ReadPcieCableEepromData&) = default;


    ReadPcieCableEepromData& operator=(ReadPcieCableEepromData&&) = default;


    virtual ~ReadPcieCableEepromData();


    virtual const char* get_command_name() const override {
        return "ReadPcieCableEepromData";
    }


    /*!
     * @brief Set PCIe slot to read EEPROM data from.
     * @param pcie_slot_id PCIe slot ID.
     */
    void set_pcie_slot_id(std::uint16_t pcie_slot_id) {
        m_pcie_slot_id = pcie_slot_id;
    }


    /*!
     * @brief Set cable port index.
     * @param cable_port_index Cable port index.
     */
    void set_cable_port_index(std::uint8_t cable_port_index) {
        m_cable_port_index = cable_port_index;
    }


    /*!
     * @brief Set upper memory region of the cable memory.
     * @param upper_memory_page_select Upper memory page select.
     */
    void set_upper_memory_page_select(std::uint8_t upper_memory_page_select) {
        m_upper_memory_page_select = upper_memory_page_select;
    }


    /*!
     * @brief Set offset of the memory data to read.
     * @param reading_data_offset Reading data offset.
     */
    void set_reading_data_offset(std::uint8_t reading_data_offset) {
        m_reading_data_offset = reading_data_offset;
    }


    /*!
     * @brief Set the length of data to read.
     * @param reading_data_length Length of data to read.
     */
    void set_reading_data_length(std::uint8_t reading_data_length) {
        m_reading_data_length = reading_data_length;
    }


private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;


    std::uint16_t m_pcie_slot_id{};
    std::uint8_t m_cable_port_index{};
    std::uint8_t m_upper_memory_page_select{};
    std::uint8_t m_reading_data_offset{};
    std::uint8_t m_reading_data_length{};
};

}

namespace response {

/*!
 * @brief ReadPcieCableEepromData response.
 */
class ReadPcieCableEepromData : public Response {
public:
    ReadPcieCableEepromData();


    ReadPcieCableEepromData(const ReadPcieCableEepromData&) = default;


    ReadPcieCableEepromData(ReadPcieCableEepromData&&) = default;


    ReadPcieCableEepromData& operator=(const ReadPcieCableEepromData&) = default;


    ReadPcieCableEepromData& operator=(ReadPcieCableEepromData&&) = default;


    virtual ~ReadPcieCableEepromData();


    virtual const char* get_command_name() const override {
        return "ReadPcieCableEepromData";
    }


    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }


    /*!
     * @brief Get length of read data.
     * @return Read data length.
     */
    std::uint8_t get_read_data_lenght() const {
        return m_read_data_length;
    }


    /*!
     * @brief Get cable memory data.
     * @return Cable memory data.
     */
    std::vector<std::uint8_t> get_cable_memory_data() const {
        return m_cable_memory_data;
    }


private:
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;


    static constexpr size_t RESPONSE_MINIMUM_SIZE = 4;
    static constexpr size_t READ_DATA_LENGTH_OFFSET = 2;
    static constexpr size_t CABLE_MEMORY_DATA_OFFSET = 3;

    bool m_is_rackscale_extension_byte_present{false};
    std::uint8_t m_read_data_length{};
    std::vector<std::uint8_t> m_cable_memory_data{};
};

}

}
}
}
}
