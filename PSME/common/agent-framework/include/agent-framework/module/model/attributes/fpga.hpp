/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

class Fpga {
public:
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
     * @brief Get bit stream version
     *
     * @return Bit stream version
     */
    const OptionalField<std::string>& get_bit_stream_version() const {
        return m_bit_stream_version;
    }


    /*!
     * @brief Set bit stream version
     *
     * @param bit_stream_version Bit stream version
     */
    void set_bit_stream_version(const OptionalField<std::string>& bit_stream_version) {
        m_bit_stream_version = bit_stream_version;
    }


    /*!
     * @brief Get HSSI configuration
     *
     * @return HSSI configuration
     */
    const OptionalField<enums::HssiConfiguration>& get_hssi_configuration() const {
        return m_hssi_configuration;
    }


    /*!
     * @brief Set HSSI configuration
     *
     * @param hssi_configuration HSSI configuration
     */
    void set_hssi_configuration(const OptionalField<enums::HssiConfiguration>& hssi_configuration) {
        m_hssi_configuration = hssi_configuration;
    }


    /*!
     * @brief Get HSSI sideband
     *
     * @return HSSI sideband
     */
    const OptionalField<enums::HssiSideband>& get_hssi_sideband() const {
        return m_hssi_sideband;
    }


    /*!
     * @brief Set HSSI sideband
     *
     * @param hssi_sideband HSSI sideband
     */
    void set_hssi_sideband(const OptionalField<enums::HssiSideband>& hssi_sideband) {
        m_hssi_sideband = hssi_sideband;
    }


    /*!
     * @brief Get FPGA reconfigration slots
     *
     * @return FPGA reconfiguration slots
     */
    const OptionalField<std::uint32_t>& get_reconfiguration_slots() const {
        return m_reconfiguration_slots;
    }


    /*!
     * @brief Set FPGA reconfiguration slots
     *
     * @param reconfiguration_slots FPGA reconfiguration slots
     */
    void set_reconfiguration_slots(const OptionalField<std::uint32_t>& reconfiguration_slots) {
        m_reconfiguration_slots = reconfiguration_slots;
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
    OptionalField<enums::FpgaType> m_type{};
    OptionalField<std::string> m_bit_stream_version{};
    OptionalField<enums::HssiConfiguration> m_hssi_configuration{};
    OptionalField<enums::HssiSideband> m_hssi_sideband{};
    OptionalField<std::uint32_t> m_reconfiguration_slots{};

};

}
}
}
