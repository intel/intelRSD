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
 * */

#include "loader/storage_loader.hpp"

#include "logger/logger.hpp"
#include "json-wrapper/json-wrapper.hpp"

extern "C" {
#include <arpa/inet.h>
#include <ifaddrs.h>
}

#include <thread>

using namespace agent::storage::loader;

namespace {

struct IfAddrsDeleter {
    void operator()(struct ifaddrs* ifa) const {
        freeifaddrs(ifa);
    }
};

using IfAddrsPointer = std::unique_ptr<struct ifaddrs, IfAddrsDeleter>;

constexpr const int WAIT_FOR_ADDRESS_SEC = 10;

static const std::string MANAGERS{"managers"};
static const std::string STORAGE_SERVICES{"storageServices"};
static const std::string TGT_SOCKET{"tgt-socket"};
static const std::string ISCSI{"iscsi"};
static const std::string PORTAL_INTERFACE{"portal-interface"};
static const std::string PORT{"port"};
static const std::string USERNAME{"username"};
static const std::string PASSWORD{"password"};
static const std::string INITIATOR{"initiator"};
static const std::string CONFIG_PATH{"config-path"};

std::string get_iface_ipaddress(const std::string& iface) {
    /* Agent is useless without portal IP, will not continue until it is set */
    while (true) {
        struct ifaddrs* ifa = nullptr;
        IfAddrsPointer ifaddrs{};
        if (0 == getifaddrs(&ifa)) {
            for (ifaddrs.reset(ifa); ifa; ifa = ifa->ifa_next) {
                if (AF_INET == ifa->ifa_addr->sa_family && 0 == iface.compare(ifa->ifa_name)) {
                    auto sock_addr = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
                    auto ip_address = inet_ntoa(sock_addr->sin_addr);
                    if (nullptr != ip_address) {
                        return ip_address;
                    }
                }
            }
        }
        log_error("storage-agent", "Could not read IP address from interface " << iface);
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_ADDRESS_SEC));
    }
}

void read_storage_service_configuration(std::shared_ptr<AgentConfiguration> configuration, const json::Json& json) {
    /* Check storage config type */
    if (!json.is_object()) {
        throw std::runtime_error("'" + STORAGE_SERVICES + "' item has invalid type.");
    }

    /* Read TGT configuration */
    if (!json.count(TGT_SOCKET) || !json[TGT_SOCKET].is_string()) {
        throw std::runtime_error("'" + TGT_SOCKET +"' item is required and has to be a string.");
    }
    else {
        configuration->set_tgt_socket(json[TGT_SOCKET].get<std::string>());
    }

    /* Read iSCSI configuration */
    if (json.count(ISCSI) && json[ISCSI].is_object()) {
        tgt::IscsiData iscsi_data{};
        const auto& iscsi_json = json[ISCSI];
        if (iscsi_json.count(PORTAL_INTERFACE) && iscsi_json[PORTAL_INTERFACE].is_string()) {
            iscsi_data.set_portal_ip(::get_iface_ipaddress(iscsi_json[PORTAL_INTERFACE].get<std::string>()));
            iscsi_data.set_portal_interface(iscsi_json[PORTAL_INTERFACE].get<std::string>());
        }

        if (iscsi_json.count(PORT) && iscsi_json[PORT].is_number_unsigned()) {
            iscsi_data.set_portal_port(iscsi_json[PORT].get<std::uint16_t>());
        }

        if (iscsi_json.count(USERNAME) && iscsi_json[USERNAME].is_string()) {
            iscsi_data.set_username(iscsi_json[USERNAME].get<std::string>());
        }

        if (iscsi_json.count(PASSWORD) && iscsi_json[PASSWORD].is_string()) {
            iscsi_data.set_password(iscsi_json[PASSWORD].get<std::string>());
        }

        if (iscsi_json.count(INITIATOR) && iscsi_json[INITIATOR].is_string()) {
            iscsi_data.set_initiator(iscsi_json[INITIATOR].get<std::string>());
        }

        if (iscsi_json.count(CONFIG_PATH) && iscsi_json[CONFIG_PATH].is_string()) {
            iscsi_data.set_configuration_path(iscsi_json[CONFIG_PATH].get<std::string>());
        }

        configuration->set_iscsi_data(std::move(iscsi_data));
    }
    else {
        throw std::runtime_error("StorageServices iSCSI data is missing");
    }
}

void read_manager_configuration(std::shared_ptr<AgentConfiguration> configuration, const json::Json& json) {
    /* Check manager config type */
    if (!json.is_object()) {
        throw std::runtime_error("'" + MANAGERS + "' item has invalid type.");
    }

    /* Check if storage services exists */
    if (json.count(STORAGE_SERVICES) && json[STORAGE_SERVICES].is_array() && json[STORAGE_SERVICES].size() > 0) {
        /* Read storage services */
        for (const auto& storage_json : json[STORAGE_SERVICES]) {
            read_storage_service_configuration(configuration, storage_json);
            log_debug("storage-agent", "StorageService configuration section loaded correctly.");
        }
    }
    else {
        throw std::runtime_error("No '" + STORAGE_SERVICES + "' key found in config file.");
    }
}

void read_configuration(std::shared_ptr<AgentConfiguration> configuration, const json::Json& json) {
    if (json.count(MANAGERS) && json[MANAGERS].is_array() && !json[MANAGERS].empty()) {
        for (const auto& manager_json : json[MANAGERS]) {
            read_manager_configuration(configuration, manager_json);
            log_debug("storage-agent", "Manager configuration section loaded correctly.");
        }
    }
    else {
        throw std::runtime_error("No '" + MANAGERS + "' key found in config file.");
    }
}

}


StorageLoader::StorageLoader() : m_configuration(std::make_shared<AgentConfiguration>()) { }



bool StorageLoader::load(const json::Json& json) {
    try {
        /* read storage configuration */
        ::read_configuration(m_configuration, json);
    }
    catch (const std::exception& e) {
        log_error("storage-agent", "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}

std::shared_ptr<AgentConfiguration> StorageLoader::get() const {
    return m_configuration;
}
