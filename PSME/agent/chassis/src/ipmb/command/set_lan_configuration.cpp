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
 * */

#include <ipmb/command/set_lan_configuration.hpp>
#include <ipmb/network_utils.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>
#include "configuration/configuration.hpp"

#include <iterator>
#include <thread>

using namespace agent::chassis::ipmb::command;

SetLanConfiguration::~SetLanConfiguration() {}

void SetLanConfiguration::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();
    data[OFFSET_CC] = uint8_t(m_cc);
    msg.add_len(get_len());
}

void SetLanConfiguration::unpack(IpmiMessage& msg) {
    log_debug("ipmb", "Unpacking Set Lan Configuration message.");
    msg.set_to_request();
    auto data = msg.get_data();
    m_param = data[OFFSET_SELECTOR];
    m_data.emplace_back(data[OFFSET_DATA]);
    m_data.emplace_back(data[OFFSET_DATA + 1]);
    m_data.emplace_back(data[OFFSET_DATA + 2]);
    m_data.emplace_back(data[OFFSET_DATA + 3]);
}

void SetLanConfiguration::pack(IpmiMessage& msg) {
    process_request();
    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
    msg.set_to_response();
}

namespace {
void restart_network(const std::string& interface) {
    std::thread([interface] () {
       std::this_thread::sleep_for(std::chrono::seconds{2});
       std::string ip_flush = "ip addr flush dev ";
       ip_flush += interface;
       auto ec = system(ip_flush.c_str());
       log_debug(LOGUSR, ip_flush << " ec: " << ec);
       ec = system("systemctl restart systemd-networkd");
       log_debug(LOGUSR, "systemctl restart systemd-networkd ec: " << ec);
    }).detach();
}
}

void SetLanConfiguration::process_request() {
    auto& config = configuration::Configuration::get_instance().to_json();
    auto& managers = config.count("managers") ? config["managers"] : json::Json{};

    for (const auto& manager : managers) {
        if (manager.value("chassis", json::Json()).is_object()) {
            if (manager["chassis"].count("networkInterface") &&
                manager["chassis"]["networkInterface"].is_string()) {
                std::string interface = manager["chassis"]["networkInterface"].get<std::string>();
                try {
                    switch (m_param) {
                        case PARAM_IP_ADDRESS: process_ip(interface); break;
                        case PARAM_IP_ADDRESS_SOURCE: process_source(interface); break;
                        case PARAM_IP_SUBNET_MASK: process_mask(interface); break;
                        default:
                            log_debug(LOGUSR, "Invalid parameter");
                            m_response.set_cc(CompletionCode::CC_PARAMETER_NOT_SUPPORTED);
                            return;
                    }
                } catch (const std::runtime_error& e) {
                    log_debug(LOGUSR, "Cannot set interface settings : " << e.what());
                    m_response.set_cc(CompletionCode::CC_CANNOT_RETURN_REQ_DATA);
                    return;
                }

                restart_network(interface);
                return;
            }
        }
    }
    log_debug(LOGUSR, "Configuration interface not set");
    m_response.set_cc(CompletionCode::CC_CANNOT_RETURN_REQ_DATA);
    return;
}

void SetLanConfiguration::process_ip(const std::string& interface) {
    log_debug(LOGUSR, "Set interface ip : " << network_utils::bytes_to_addr(m_data));
    network_utils::set_ip(interface, network_utils::bytes_to_addr(m_data));
}
void SetLanConfiguration::process_source(const std::string& interface) {
    log_debug(LOGUSR, "Set IP address source...");
    network_utils::set_source(interface,
                                        m_data[0] == 1 ?
                                        network_utils::SourceOption::STATIC :
                                        network_utils::SourceOption::DHCP);
}
void SetLanConfiguration::process_mask(const std::string& interface) {
    log_debug(LOGUSR, "Set interface mask : " << network_utils::bytes_to_addr(m_data));
    network_utils::set_mask(interface, network_utils::bytes_to_addr(m_data));
}

