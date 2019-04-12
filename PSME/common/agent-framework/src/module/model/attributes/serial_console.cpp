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
 * */

#include "agent-framework/module/model/attributes/serial_console.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

SerialConsole::SerialConsole() { }

SerialConsole::~SerialConsole() { }

json::Json SerialConsole::to_json() const {
    json::Json result = json::Json();
    result[literals::SerialConsole::SIGNAL_TYPE] = get_signal_type();
    result[literals::SerialConsole::BITRATE] = get_bitrate();
    result[literals::SerialConsole::PARITY] = get_parity();
    result[literals::SerialConsole::DATA_BITS] = get_data_bits();
    result[literals::SerialConsole::STOP_BITS] = get_stop_bits();
    result[literals::SerialConsole::FLOW_CONTROL] = get_flow_control();
    result[literals::SerialConsole::PIN_OUT] = get_pin_out();
    result[literals::SerialConsole::ENABLED] = get_enabled();
    result[literals::SerialConsole::MAX_SESSIONS] = get_max_sessions();
    result[literals::SerialConsole::TYPES_SUPPORTED] = get_types_supported().to_json();
    return result;
}

SerialConsole SerialConsole::from_json(const json::Json& json) {
    attribute::SerialConsole serial_console;
    serial_console.set_signal_type(OptionalField<enums::SerialConsoleSignalType>(
        json[literals::SerialConsole::SIGNAL_TYPE]));
    serial_console.set_bitrate(json[literals::SerialConsole::BITRATE]);
    serial_console.set_parity(OptionalField<enums::SerialConsoleParity>(
        json[literals::SerialConsole::PARITY]));
    serial_console.set_data_bits(json[literals::SerialConsole::DATA_BITS]);
    serial_console.set_stop_bits(json[literals::SerialConsole::STOP_BITS]);
    serial_console.set_flow_control(OptionalField<enums::SerialConsoleFlowControl>(
        json[literals::SerialConsole::FLOW_CONTROL]));
    serial_console.set_pin_out(OptionalField<enums::SerialConsolePinOut>(
        json[literals::SerialConsole::PIN_OUT]));
    serial_console.set_enabled(json[literals::SerialConsole::ENABLED]);
    serial_console.set_max_sessions(json[literals::SerialConsole::MAX_SESSIONS]);
    serial_console.set_types_supported(SupportedConnections::from_json(
        json[literals::SerialConsole::TYPES_SUPPORTED]));
    return serial_console;
}
