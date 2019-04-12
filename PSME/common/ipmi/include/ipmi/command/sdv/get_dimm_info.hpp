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
 * @file get_dimm_info.hpp
 *
 * @brief GetDimmInfo IPMI command
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for Get DIMM Info. Need to set DIMM slot number.
 */
class GetDimmInfo : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetDimmInfo();

    /*! Copy constructor. */
    GetDimmInfo(const GetDimmInfo&) = default;

    /*! Assignment operator */
    GetDimmInfo& operator=(const GetDimmInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetDimmInfo();

    /*!
     * @brief Sets memory index (slot number).
     * @param index memory slot number/index.
     */
    void set_dimm_index(std::uint8_t index) {
        m_dimm_index = index;
    }

    virtual void pack(std::vector<std::uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetDimmInfo";
    }

private:
    std::uint8_t m_dimm_index{};

};

}

namespace response {

/*!
 * @brief Response message for Get DIMM Info command. Gets memory properties like: size, speed, type.
 */
class GetDimmInfo : public Response {
public:

    /*!
     * @brief Represents codes for DIMM Voltage Type.
     */
    enum DIMM_VOLTAGE : std::uint8_t {
        DIMM_VOLTAGE_NORMAL = 0x00,
        DIMM_VOLTAGE_ULTRA_LOW = 0x01,
        DIMM_VOLTAGE_LOW = 0x02,
        DIMM_VOLTAGE_DDR4_NORMAL = 0x03,
        DIMM_VOLTAGE_LAST,
        DIMM_VOLTAGE_UNKNOWN,
    };

    /*!
     * @brief Represents codes for DIMM Type.
     */
    enum DIMM_TYPE : std::uint8_t {
        DIMM_TYPE_SDRAM = 0x00,
        DIMM_TYPE_DDR1 = 0x01,
        DIMM_TYPE_RAMBUS = 0x02,
        DIMM_TYPE_DDR2 = 0x03,
        DIMM_TYPE_FBDIMM = 0x04,
        DIMM_TYPE_DDR3 = 0x05,
        DIMM_TYPE_DDR4 = 0x06,
        DIMM_TYPE_NO_DIMM = 0x3F,
        DIMM_TYPE_LAST,
        DIMM_TYPE_UNKNOWN,
    };

    /*!
     * @brief Represents codes for DIMM Presence.
     */
    enum DIMM_PRESENCE : std::uint8_t {
        DIMM_PRESENCE_PRESENT = 0x02,
        DIMM_PRESENCE_NOT_PRESENT = 0x03,
        DIMM_PRESENCE_LAST,
        DIMM_PRESENCE_UNKNOWN
    };

    /*!
     * @brief Default constructor.
     */
    GetDimmInfo();

    /*! Copy constructor. */
    GetDimmInfo(const GetDimmInfo&) = default;

    /*! Assignment operator */
    GetDimmInfo& operator=(const GetDimmInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetDimmInfo();

    /*!
     * @brief Gets DIMM type. Like DDR-4 or Rambus.
     * @return string with DIMM type name.
     */
    std::string get_dimm_type() const {
        return m_dimm_type;
    }

    /*!
     * @brief Gets DIMM voltage.
     * @return float with voltage number and name.
     */
    double get_dimm_voltage() const {
        return m_dimm_voltage;
    }

    /*!
     * @brief Gets DIMM speed in MHz.
     * @return float with memory speed in MHz.
     */
    std::uint32_t get_dimm_speed_mhz() const {
        return m_dimm_speed_mhz;
    }

    /*!
     * @brief Gets DIMM size in MB.
     * @return uint32_t with memory size in MB.
     */
    std::uint32_t get_dimm_size_mbytes() const {
        return m_dimm_size_mbytes;
    }

    /*!
     * @brief Checks if DIMM slot is occupied.
     * @return true if memory slot is occupied, otherwise false.
     */
    bool is_present() const {
        return m_dimm_presence;
    }

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "GetDimmInfo";
    }

private:
    std::string m_dimm_type{};
    double m_dimm_voltage{};
    std::uint32_t m_dimm_speed_mhz{};
    std::uint32_t m_dimm_size_mbytes{};
    bool m_dimm_presence{};

    static constexpr std::size_t XEON_RESPONSE_SIZE = 9;
    static constexpr std::size_t ATOM_RESPONSE_SIZE = 7;

    static constexpr std::size_t XEON_OFFSET_PRESENCE = 8;
    static constexpr std::size_t ATOM_OFFSET_PRESENCE = 6;

    static constexpr std::size_t OFFSET_DIMM_TYPE_AND_VOLTAGE = 1;
    static constexpr std::size_t OFFSET_DIMM_SPEED = 2;
    static constexpr std::size_t OFFSET_DIMM_SIZE = 4;

    static constexpr std::size_t MASK_DIMM_VOLTAGE = 0xC0;
    static constexpr std::size_t MASK_DIMM_TYPE = 0x3F;

    std::map<DIMM_VOLTAGE, double> m_voltage_mapping = {
        {DIMM_VOLTAGE_NORMAL, 1.5},
        {DIMM_VOLTAGE_ULTRA_LOW, 1.25},
        {DIMM_VOLTAGE_LOW, 1.35},
        {DIMM_VOLTAGE_DDR4_NORMAL, 1.2},
        {DIMM_VOLTAGE_UNKNOWN, 0.0},
    };

    std::map<DIMM_TYPE, std::string> m_type_mapping = {
        {DIMM_TYPE_SDRAM, "SDRAM"},
        {DIMM_TYPE_DDR1, "DDR1"},
        {DIMM_TYPE_RAMBUS, "Rambus"},
        {DIMM_TYPE_DDR2, "DDR2"},
        {DIMM_TYPE_FBDIMM, "FBDIMM"},
        {DIMM_TYPE_DDR3, "DDR3"},
        {DIMM_TYPE_DDR4, "DDR4"},
        {DIMM_TYPE_NO_DIMM, "No DIMM present"},
        {DIMM_TYPE_UNKNOWN, "DIMM type unknown"},
    };

    std::string type_to_string(std::uint8_t value) const;
    double extract_voltage(std::uint8_t value) const;
    std::uint8_t extract_type(std::uint8_t value) const;
    std::uint32_t extract_speed(const std::vector<std::uint8_t>& data) const;

    bool is_xeon(const std::vector<std::uint8_t>& data) const;
    bool is_atom(const std::vector<std::uint8_t>& data) const;

    void common_unpack(const std::vector<std::uint8_t>& data);
    void xeon_unpack(const std::vector<std::uint8_t>& data);
    void atom_unpack(const std::vector<std::uint8_t>& data);

    std::uint16_t atom_extract_size(const std::vector<std::uint8_t>& data) const;
    std::uint32_t xeon_extract_size(const std::vector<std::uint8_t>& data) const;
    bool atom_extract_presence(const std::vector<std::uint8_t>& data) const;
    bool xeon_extract_presence(const std::vector<std::uint8_t>& data) const;

};
}

}
}
}
