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
 * @file serial_console.hpp
 * @brief Declaration of SerialConsole attribute
 * */

#pragma once

#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

class SerialConsole {
public:
    using SupportedConnections = Array<enums::SerialConsoleSupportedType>;

    explicit SerialConsole();

    ~SerialConsole();

    /*! Enable copy */
    SerialConsole(const SerialConsole&) = default;
    SerialConsole& operator=(const SerialConsole&) = default;
    SerialConsole(SerialConsole&&) = default;
    SerialConsole& operator=(SerialConsole&&) = default;

    /*!
     * @brief Get signal type
     * @return signal type
     * */
    const OptionalField<enums::SerialConsoleSignalType>& get_signal_type() const {
        return m_signal_type;
    }

    /*!
     * @brief Get parity
     * @return parity
     * */
    const OptionalField<enums::SerialConsoleParity>& get_parity() const {
        return m_parity;
    }

    /*!
     * @brief Get flow control
     * @return flow control
     * */
    const OptionalField<enums::SerialConsoleFlowControl>& get_flow_control() const {
        return m_flow_control;
    }

    /*!
     * @brief Get pin out
     * @return pin out
     * */
    const OptionalField<enums::SerialConsolePinOut>& get_pin_out() const {
        return m_pin_out;
    }

    /*!
     * @brief Get bitrate
     * @return bitrate
     * */
    const OptionalField<uint32_t>& get_bitrate() const {
        return m_bitrate;
    }

    /*!
     * @brief Get data bits
     * @return data bits
     * */
    const OptionalField<uint32_t>& get_data_bits() const {
        return m_data_bits;
    }

    /*!
     * @brief Get stop bits
     * @return stop bits
     * */
    const OptionalField<uint32_t>& get_stop_bits() const {
        return m_stop_bits;
    }

    /*!
     * @brief Is enabled
     * @return true if manager is enabled, false otherwise
     * */
    const OptionalField<bool>& get_enabled() const {
        return m_enabled;
    }

    /*!
     * @brief Set signal type
     * @param[in] signal_type Signal type
     * */
    void set_signal_type(const OptionalField<enums::SerialConsoleSignalType>& signal_type) {
        m_signal_type = signal_type;
    }

    /*!
     * @brief Set parity
     * @param[in] parity Parity
     * */
    void set_parity(const OptionalField<enums::SerialConsoleParity>& parity) {
        m_parity = parity;
    }

    /*!
     * @brief Set flow control
     * @param[in] flow_control Flow control
     * */
    void set_flow_control(const OptionalField<enums::SerialConsoleFlowControl>& flow_control) {
        m_flow_control = flow_control;
    }

    /*!
     * @brief Set pin out
     * @param[in] pin_out Pin out
     * */
    void set_pin_out(const OptionalField<enums::SerialConsolePinOut>& pin_out) {
        m_pin_out = pin_out;
    }

    /*!
     * @brief Set bitrate
     * @param bitrate Bitrate
     * */
    void set_bitrate(const OptionalField<uint32_t>& bitrate) {
        m_bitrate = bitrate;
    }

    /*!
     * @brief Data bits
     * @param data_bits Data bits
     * */
    void set_data_bits(const OptionalField<uint32_t>& data_bits) {
        m_data_bits = data_bits;
    }

    /*!
     * @brief Stop bits
     * @param stop_bits Stop bits
     * */
    void set_stop_bits(const OptionalField<uint32_t>& stop_bits) {
        m_stop_bits = stop_bits;
    }

    /*!
     * @brief Set enabled
     * @param enabled Enabled
     * */
    void set_enabled(const OptionalField<bool>& enabled) {
        m_enabled = enabled;
    }

    /*!
     * @brief Get number of session that can be established at the same time (optional)
     * @return Number of allowed sessions or NIL if not set
     */
    const OptionalField<std::uint32_t>& get_max_sessions() const {
        return m_max_sessions;
    }
    /*!
     * @brief Set number of session that can be established at the same time (optional)
     * @param max_sessions number of allowed sessions or NIL to remove
     */
    void set_max_sessions(const OptionalField<std::uint32_t>& max_sessions) {
        m_max_sessions = max_sessions;
    }

    /*!
     * @brief Get supported types of connection (should be optional)
     * @return array with supported types of connections
     */
    const SupportedConnections& get_types_supported() const {
        return m_types_supported;
    }

    /*!
     * @brief Set supported types of connection (should be optional)
     * @param[in] types_supported array with supported types of connections
     */
    void set_types_supported(const SupportedConnections& types_supported) {
        m_types_supported = types_supported;
    }

    /*!
     * @brief construct an object of class SerialConsole from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed SerialConsole object
     */
    static SerialConsole from_json(const json::Json& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

private:

    OptionalField<enums::SerialConsoleSignalType> m_signal_type{};
    OptionalField<uint32_t> m_bitrate{};
    OptionalField<enums::SerialConsoleParity> m_parity{};
    OptionalField<uint32_t> m_data_bits{};
    OptionalField<uint32_t> m_stop_bits{};
    OptionalField<enums::SerialConsoleFlowControl> m_flow_control{};
    OptionalField<enums::SerialConsolePinOut> m_pin_out{};
    OptionalField<bool> m_enabled{};
    OptionalField<uint32_t> m_max_sessions{};
    SupportedConnections m_types_supported{};
};

}
}
}
