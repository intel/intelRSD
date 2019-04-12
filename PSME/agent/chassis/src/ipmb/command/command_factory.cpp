/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file ipmb/message_handler.cpp
 * @brief IPMI message handler.
 * */

#include <ipmb/command/command.hpp>
#include <ipmb/command/command_factory.hpp>
#include <ipmb/command/get_sled_presence.hpp>
#include <ipmb/command/get_device_id.hpp>
#include <ipmb/command/warm_reset.hpp>
#include <ipmb/command/get_aggregated_health_sensor.hpp>
#include <ipmb/command/get_aggregated_thermal_sensor.hpp>
#include <ipmb/command/get_drawer_power.hpp>
#include <ipmb/command/set_id_field.hpp>
#include <ipmb/command/get_id_field.hpp>
#include <ipmb/command/get_lan_configuration.hpp>
#include <ipmb/command/set_lan_configuration.hpp>
#include <ipmb/command/get_authorization_certificate.hpp>
#include <ipmb/command/get_authorization_certificate_hash.hpp>
#include <ipmb/command/set_authorization_certificate.hpp>
#include <ipmb/command/unknown_command.hpp>

#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb::command;

CommandUniquePtr CommandFactory::get_command(const uint8_t lun,
        const NetFn netfn, const CmdCode cmd_code) {

    CommandUniquePtr cmd {};

    if (IPMI_MSG_DEFAULT_LUN == lun) {
        if (NetFn::IPMI_NETFN_APP == netfn) {
            switch (cmd_code) {
            case CmdCode::GET_DEV_ID:
                log_debug(LOGUSR, "Execute Get Device ID IPMI command.");
                cmd.reset(new GetDeviceID());
                break;
            case CmdCode::WARM_RESET:
                log_debug(LOGUSR, "Execute WarmReset IPMI command.");
                cmd.reset(new WarmReset());
                break;
            default:
                break;
            }
        }
        else if (NetFn::IPMI_NETFN_INTEL_REQ == netfn) {
            switch(cmd_code) {
            case CmdCode::GET_AGGR_HEALTH_SENSOR:
                log_debug(LOGUSR, "Execute Get Aggregated Health Sensor IPMI command.");
                cmd.reset(new GetAggregatedHealthSensor());
                break;
            case CmdCode::GET_AGGR_THERMAL_SENSOR:
                log_debug(LOGUSR, "Execute Get Aggregated Thermal Sensor IPMI command.");
                cmd.reset(new GetAggregatedThermalSensor());
                break;
            case CmdCode::GET_TRAY_POWER:
                log_debug(LOGUSR, "Execute Get Drawer Power IPMI command.");
                cmd.reset(new GetDrawerPower());
                break;
            case CmdCode::GET_ID_FIELD:
                log_debug(LOGUSR, "Execute Get ID Field IPMI command.");
                cmd.reset(new GetIdField());
                break;
            case CmdCode::SET_ID_FIELD:
                log_debug(LOGUSR, "Execute Set ID Field IPMI command.");
                cmd.reset(new SetIdField());
                break;
            case CmdCode::SET_AUTHORIZED_CERT:
                log_debug(LOGUSR, "Execute Set Authorization Certificate IPMI command.");
                cmd.reset(new SetAuthorizationCertificate());
                break;
            case CmdCode::GET_AUTHORIZED_CERT:
                log_debug(LOGUSR, "Execute Get Authorization Certificate IPMI command.");
                cmd.reset(new GetAuthorizationCertificate());
                break;
            case CmdCode::GET_AUTHORIZED_CERT_HASH:
                log_debug(LOGUSR, "Execute Get Authorization Certificate Hash IPMI command.");
                cmd.reset(new GetAuthorizationCertificateHash());
                break;
            default:
                break;
            }
        }
        else if (NetFn::IPMI_NETFN_TRANSPORT == netfn) {
            switch(cmd_code) {
            case CmdCode::GET_LAN_CONFIGURATION:
                log_debug(LOGUSR, "Execute Get Lan Configuration IPMI command.");
                cmd.reset(new GetLanConfiguration());
                break;
            case CmdCode::SET_LAN_CONFIGURATION:
                log_debug(LOGUSR, "Execute Set Lan Configration IPMI command.");
                cmd.reset(new SetLanConfiguration());
                break;
            default:
                break;
            }
        }
    }
    else if (IPMI_MSG_OEM_REQ_LUN == lun) {
        if (NetFn::IPMI_NETFN_INTEL_RSP == netfn) {
            switch(cmd_code) {
            case CmdCode::GET_SLED_PRESENCE:
                cmd.reset(new GetSledPresence());
                break;
            default:
                break;
            }
        }
    }

    if (!cmd) {
        log_warning(LOGUSR, "Unrecognized Command lun: 0x" << std::hex << int(lun)
                             << " netfn: 0x" << int(netfn) << " cmd: 0x" << int(cmd_code));
        cmd.reset(new UnknownCommand());
    }

    return cmd;
}
