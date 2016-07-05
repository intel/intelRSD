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
 * @file get_dimm_info.hpp
 *
 * @brief GetDimmInfo IPMI command
 * */

#ifndef IPMI_COMMAND_SDV_GET_DIMM_INFO_HPP
#define	IPMI_COMMAND_SDV_GET_DIMM_INFO_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <iostream>

using std::uint8_t;
using std::uint32_t;
using std::vector;
using std::map;
using std::string;

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for Get DIMM Info. Need to set DIMM slot number.
 */
class GetDimmInfo: public Request {
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
    void set_dimm_index(uint8_t index) {
        m_dimm_index = index;
    }

    virtual void pack(vector<uint8_t>& data) const;

private:
    uint8_t m_dimm_index{};

};

}

namespace response {

/*!
 * @brief Response message for Get DIMM Info command. Gets memory properties like: size, speed, type.
 */
class GetDimmInfo: public Response {
public:

    /*!
     * @brief Represents codes for DIMM Voltage Type.
     */
    enum DIMM_VOLTAGE: uint8_t {
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
    enum DIMM_TYPE: uint8_t {
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
    enum DIMM_PRESENCE: uint8_t {
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
    string get_dimm_type() const {
        return m_dimm_type;
    }

    /*!
     * @brief Sets dimm type.
     * @param dimm_type of dimm.
     */
    void set_dimm_type(const string dimm_type){
        m_dimm_type = dimm_type;
    }

    /*!
     * @brief Gets DIMM voltage.
     * @return float with voltage number and name.
     */
    double get_dimm_voltage() const {
        return m_dimm_voltage;
    }

    /*!
     * @brief Sets dimm voltage.
     * @param dimm_voltage of dimm.
     */
    void set_dimm_voltage(const double dimm_voltage){
        m_dimm_voltage = dimm_voltage;
    }

    /*!
     * @brief Gets DIMM speed in MHz.
     * @return float with memory speed in MHz.
     */
    uint32_t get_dimm_speed_mhz() const {
        return m_dimm_speed_mhz;
    }

    /*!
     * @brief Sets dimm speed.
     * @param dimm_speed_mhz of dimm.
     */
    void set_dimm_speed_mhz(const uint32_t dimm_speed_mhz){
        m_dimm_speed_mhz = dimm_speed_mhz;
    }

    /*!
     * @brief Gets DIMM size in MB.
     * @return uint32_t with memory size in MB.
     */
    uint32_t get_dimm_size_mbytes() const {
        return m_dimm_size_mbytes;
    }

    /*!
     * @brief Sets dimm size.
     * @param dimm_size_mbytes of dimm.
     */
    void set_dimm_size_mbytes(const uint32_t dimm_size_mbytes){
        m_dimm_size_mbytes = dimm_size_mbytes;
    }

    /*!
     * @brief Checks if DIMM slot is occupied.
     * @return true if memory slot is occupied, otherwise false.
     */
    bool is_present() const {
        return m_dimm_presence;
    }

    /*!
     * @brief Sets dimm presence.
     * @param dimm_presence of dimm.
     */
    void set_dimm_presence(const bool dimm_presence) {
        m_dimm_presence = dimm_presence;

    }

    virtual void unpack(const vector<uint8_t>& data);

private:
    string m_dimm_type{};
    double m_dimm_voltage{};
    uint32_t m_dimm_speed_mhz{};
    uint32_t m_dimm_size_mbytes{};
    bool m_dimm_presence{};

    static constexpr size_t XEON_RESPONSE_SIZE = 9;
    static constexpr size_t ATOM_RESPONSE_SIZE = 7;

    static constexpr size_t XEON_OFFSET_PRESENCE = 8;
    static constexpr size_t ATOM_OFFSET_PRESENCE = 6;

    static constexpr size_t OFFSET_DIMM_TYPE_AND_VOLTAGE = 1;
    static constexpr size_t OFFSET_DIMM_SPEED = 2;
    static constexpr size_t OFFSET_DIMM_SIZE = 4;

    static constexpr size_t MASK_DIMM_VOLTAGE = 0xC0;
    static constexpr size_t MASK_DIMM_TYPE = 0x3F;

    map<DIMM_VOLTAGE, double> m_voltage_mapping = {
        {DIMM_VOLTAGE_NORMAL, 1.5},
        {DIMM_VOLTAGE_ULTRA_LOW, 1.25},
        {DIMM_VOLTAGE_LOW, 1.35},
        {DIMM_VOLTAGE_DDR4_NORMAL, 1.2},
        {DIMM_VOLTAGE_UNKNOWN, 0.0},
    };

    map<DIMM_TYPE, string> m_type_mapping = {
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

    string type_to_string(uint8_t value) const;
    double extract_voltage(uint8_t value) const;
    uint8_t extract_type(uint8_t value) const;
    uint32_t extract_speed(const vector<uint8_t>& data) const;

    /*!
     * Checks if Response is response error. If IPMI return error the data length is equal to 1.
     *
     * @param data vector with data to check.
     * @return true if Response is error, otherwise false.
     */
    bool is_error(const vector<uint8_t>& data) const;
    bool is_xeon(const vector<uint8_t>& data) const;
    bool is_atom(const vector<uint8_t>& data) const;

    void common_unpack(const vector<uint8_t>& data);
    void xeon_unpack(const vector<uint8_t>& data);
    void atom_unpack(const vector<uint8_t>& data);

    uint16_t atom_extract_size(const vector<uint8_t>& data) const;
    uint32_t xeon_extract_size(const vector<uint8_t>& data) const;
    bool atom_extract_presence(const vector<uint8_t>& data) const;
    bool xeon_extract_presence(const vector<uint8_t>& data) const;

};
}

}
}
}
#endif	/* IPMI_COMMAND_SDV_GET_DIMM_INFO_HPP */
