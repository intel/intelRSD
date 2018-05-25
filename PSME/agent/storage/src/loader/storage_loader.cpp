/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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


#include "agent-framework/module/storage_components.hpp"

#include "loader/storage_loader.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <thread>

using namespace agent::storage::loader;
using namespace agent_framework::module;
using namespace agent_framework::generic;
using namespace agent_framework::module::managers;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

namespace {
constexpr const int WAIT_FOR_ADDRESS_SEC = 10;
}

void StorageLoader::IfAddrsDeleter::operator()(struct ifaddrs *ifa) const {
    freeifaddrs(ifa);
}

const std::string StorageLoader::get_iface_ipaddress(const std::string& iface) const {
    /* Agent is useless without portal IP, will not continue until it is set */
    while (true) {
        struct ifaddrs *ifa = nullptr;
        IfAddrsPointer ifaddrs{};
        if (0 == getifaddrs(&ifa)) {
            for (ifaddrs.reset(ifa); ifa; ifa = ifa->ifa_next) {
                if (AF_INET == ifa->ifa_addr->sa_family
                    && 0 == iface.compare(ifa->ifa_name)) {
                    auto sock_addr = reinterpret_cast<
                                        struct sockaddr_in *>(ifa->ifa_addr);
                    auto ipaddress = inet_ntoa(sock_addr->sin_addr);
                    if (nullptr != ipaddress) {
                        return ipaddress;
                    }
                }
            }
        }
        log_error("storage-agent", "Could not read IP address from interface " << iface);
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_ADDRESS_SEC));
    }
}

void StorageLoader::read_configuration(const json::Value& json) {
    if (json.is_member("managers") && json["managers"].is_array() && !json["managers"].as_array().empty()) {
        for (const auto& manager_json : json["managers"].as_array()) {
            read_manager_configuration(manager_json);
            log_debug("storage-agent", "Manager configuration section loaded correctly.");
        }
    }
    else {
        throw std::runtime_error("No Managers found in config file.");
    }
}

void StorageLoader::read_manager_configuration(const json::Value& json) {

    /* check manager config type */
    if (!json.is_object()) {
        throw std::runtime_error("'managers' field invalid type");
    }

    /* check if storage services exists */
    std::string storage_name{"storageServices"};
    if (json.is_member(storage_name) && json[storage_name].is_array() && !json[storage_name].as_array().empty()) {
        /* read storage services */
        for (const auto& storage_json : json[storage_name].as_array()) {
            read_storage_service_configuration(storage_json);
            log_debug("storage-agent", "StorageService configuration section loaded correctly.");
        }
    }
    else {
        throw std::runtime_error("No StorageServices found in config file");
    }
}

void StorageLoader::read_storage_service_configuration(const json::Value& json) {
    /* check storage config type */
    if (!json.is_object()) {
        throw std::runtime_error("'storageServices' field invalid type");
    }

    /* read iscsi configuration */
    if (json.is_member("iscsi") && json["iscsi"].is_object()) {
        IscsiData iscsi_data{};
        const auto& iscsi_json = json["iscsi"];
        if (iscsi_json.is_member("portal-interface") && iscsi_json["portal-interface"].is_string()) {
            iscsi_data.set_portal_ip(get_iface_ipaddress(iscsi_json["portal-interface"].as_string()));
            iscsi_data.set_portal_interface(iscsi_json["portal-interface"].as_string());
        }

        if (iscsi_json.is_member("port") && iscsi_json["port"].is_uint()) {
            iscsi_data.set_portal_port(iscsi_json["port"].as_uint());
        }

        if (iscsi_json.is_member("username") && iscsi_json["username"].is_string()) {
            iscsi_data.set_username(iscsi_json["username"].as_string());
        }

        if (iscsi_json.is_member("password") && iscsi_json["password"].is_string()) {
            iscsi_data.set_password(iscsi_json["password"].as_string());
        }

        if (iscsi_json.is_member("initiator") && iscsi_json["initiator"].is_string()) {
            iscsi_data.set_initiator(iscsi_json["initiator"].as_string());
        }

        if (iscsi_json.is_member("config-path") && iscsi_json["config-path"].is_string()) {
            iscsi_data.set_configuration_path(iscsi_json["config-path"].as_string());
        }

        auto& iscsi_manager = get_manager<IscsiTarget, IscsiTargetManager>();
        iscsi_manager.set_iscsi_data(std::move(iscsi_data));
    }
    else {
        throw std::runtime_error("StorageServices iSCSI data is missing");
    }
}

bool StorageLoader::load(const json::Value& json) {
    try {
        /* read storage configuration */
        read_configuration(json);
    }
    catch (const std::exception& e) {
        log_error("storage-agent",
                "Load module configuration failed: " << e.what());
    }
    return true;
}
