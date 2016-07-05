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
 * @file memory.hpp
 * @brief Memory model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_MEMORY_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_MEMORY_HPP

#include "agent-framework/module/resources.hpp"
#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/fru_info.hpp"

namespace agent_framework {
namespace generic {

/*! Memory */
class Memory : public Resources {
public:
    Memory() = default;
    Memory(const Memory&) = default;
    Memory& operator=(const Memory&) = default;
    Memory(Memory&&) = default;
    Memory& operator=(Memory&&) = default;
    ~Memory();
    /*!
     * Sets socket
     *
     * @param socket Socket
     * */
    void set_socket(const std::string& socket) {
        m_socket = socket;
    }

    const std::string& get_socket() const {
        return m_socket;
    }

    /*!
     * Sets memory type
     *
     * @param type Memory type
     * */
    void set_type(const std::string& type) {
        m_type = type;
    }

    const std::string& get_type() const {
        return m_type;
    }

    /*!
     * Sets memory size in GB
     *
     * @param size_gb Memory size
     */
    void set_size_gb(std::uint32_t size_gb) {
        m_size_gb = size_gb;
    }

    std::uint32_t get_size_gb() const {
        return m_size_gb;
    }

    /*!
     * Sets memory speed in Mhz
     *
     * @param speed Memory speed
     */
    void set_speed_mhz(std::uint32_t speed) {
        m_speed_mhz = speed;
    }

    std::uint32_t get_speed_mhz() const {
        return m_speed_mhz;
    }

    /*!
     * Sets memory voltage
     *
     * @param voltage_volt Memory voltage
     * */
    void set_voltage_volt(double voltage_volt) {
        m_voltage_volt = voltage_volt;
    }

    double get_voltage_volt() const {
        return m_voltage_volt;
    }

    /*!
     * Sets data width in bits
     *
     * @param data_width_bits Data width in bits
     * */
    void set_data_width_bits(std::uint32_t data_width_bits) {
        m_data_width_bits = data_width_bits;
    }

    std::uint32_t get_data_width_bits() const {
        return m_data_width_bits;
    }
    /*!
     * Sets total width in bits
     *
     * @param total_width_bits Total width in bits
     * */
    void set_total_width_bits(std::uint32_t total_width_bits) {
        m_total_width_bits = total_width_bits;
    }

    std::uint32_t get_total_width_bits() const {
        return m_total_width_bits;
    }

    /*!
     * Sets form factor
     *
     * @param form_factor Form factor (DIMM, SODIMM, ...)
     * */
    void set_form_factor(const std::string& form_factor) {
        m_form_factor = form_factor;
    }

    const std::string& get_form_factory() const {
        return m_form_factor;
    }

    /*!
     * @brief Set FruInfo for response
     * @param[in] fru_info FruInfo
     * */
    void set_fru_info(const generic::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    const generic::FruInfo& get_fru_info() const {
        return m_fru_info;
    }


    /*!
     * @brief Set OEM specific data for response
     * @param[in] oem OEM specific data
     * */
    void set_oem(const generic::OEMData& oem) {
        m_oem = oem;
    }

    const generic::OEMData& get_eom() const {
        return m_oem;
    }

private:
    std::string m_socket{};
    std::string m_type{};
    std::uint32_t m_size_gb{};
    std::uint32_t m_speed_mhz{};
    double m_voltage_volt{};
    std::uint32_t m_data_width_bits{};
    std::uint32_t m_total_width_bits{};
    std::string m_form_factor{};

    generic::FruInfo m_fru_info{};
    generic::OEMData m_oem{};

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MEMORY_HPP */
