/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_api.cpp
 */

#include "spdk/spdk_api.hpp"

#include "json-rpc/connectors/unix_domain_socket_client_connector.hpp"

#include "logger/logger.hpp"



using namespace spdk;

namespace {

constexpr const char LVS_NAME[] = "lvs_name";
constexpr const char BDEV_NAME[] = "bdev_name";
constexpr const char THIN_PROVISION[] = "thin_provision";
constexpr const char CLEAR_METHOD[] = "clear_method";
constexpr const char UUID[] = "uuid";
constexpr const char NAME[] = "name";
constexpr const char SIZE[] = "size";
constexpr const char LVOL_NAME[] = "lvol_name";
constexpr const char STRIP_SIZE[] = "strip_size";
constexpr const char RAID_LEVEL[] = "raid_level";
constexpr const char BASE_BDEVS[] = "base_bdevs";
constexpr const char CLUSTER_SIZE[] = "cluster_sz";
constexpr const char CATEGORY[] = "category";
constexpr const char SNAPSHOT_NAME[] = "snapshot_name";
constexpr const char CLONE_NAME[] = "clone_name";

}
constexpr const char spdk::Method::GET_RPC_METHODS[];

constexpr const char spdk::Method::GET_BDEVS[];
constexpr const char spdk::Method::GET_LVOL_STORES[];
constexpr const char spdk::Method::GET_NVMF_SUBSYSTEMS[];
constexpr const char spdk::Method::GET_RAID_BDEVS[];

constexpr const char spdk::Method::CONSTRUCT_LVOL_STORE[];
constexpr const char spdk::Method::CONSTRUCT_LVOL_BDEV[];
constexpr const char spdk::Method::CONSTRUCT_NVMF_SUBSYSTEM[];
constexpr const char spdk::Method::NVMF_SUBSYSTEM_CREATE[];
constexpr const char spdk::Method::CONSTRUCT_RAID_BDEV[];

constexpr const char spdk::Method::NVMF_SUBSYSTEM_REMOVE_HOST[];
constexpr const char spdk::Method::NVMF_SUBSYSTEM_ADD_HOST[];
constexpr const char spdk::Method::NVMF_SUBSYSTEM_ADD_LISTENER[];
constexpr const char spdk::Method::NVMF_SUBSYSTEM_ADD_NS[];

constexpr const char spdk::Method::DESTROY_LVOL_STORE[];
constexpr const char spdk::Method::DESTROY_LVOL_BDEV[];
constexpr const char spdk::Method::DELETE_NVMF_SUBSYSTEM[];
constexpr const char spdk::Method::DESTROY_RAID_BDEV[];

constexpr const char spdk::Method::RESIZE_LVOL_BDEV[];
constexpr const char spdk::Method::SNAPSHOT_LVOL_BDEV[];
constexpr const char spdk::Method::CLONE_LVOL_BDEV[];

constexpr const char spdk::Method::INFLATE_LVOL_BDEV[];


SpdkApi::SpdkApi(const json_rpc::AbstractClientConnectorPtr connector,
                 std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker)
    : m_connector(connector),
      m_invoker(invoker) {}


const json::Json& SpdkApi::send_request(const std::string& method_name, const json::Json& parameters) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_invoker->prepare_method(method_name, parameters);

    log_debug("spdk-api", "Request to " << method_name << ":\n" << parameters.dump(2));
    try {
        m_invoker->call(m_connector);
    }
    catch (const json_rpc::JsonRpcException& exception) {
        // Hiding JSON-RPC implementation
        log_debug("spdk-api", "Exception occurred for " << method_name << ":\n" << exception.what());
        throw std::runtime_error(exception.get_message());
    }

    const auto& result = m_invoker->get_result();
    log_debug("spdk-api", "Result for " << method_name << ":\n" << result.dump(2));
    return result;
}


std::vector<std::string> SpdkApi::get_rpc_methods() {
    return send_request(Method::GET_RPC_METHODS);
}


std::vector<spdk::model::Bdev> SpdkApi::get_bdevs(const JsonOptional<std::string>& name) {
    json::Json parameters{};

    if (name) {
        parameters[model::Bdev::NAME] = name;
    }

    return send_request(spdk::Method::GET_BDEVS, parameters);
}


std::vector<spdk::model::LvolStore> SpdkApi::get_lvol_stores() {
    return send_request(Method::GET_LVOL_STORES);
}


std::vector<spdk::model::LvolStore> SpdkApi::get_lvol_stores_by_uuid(const std::string& uuid) {
    json::Json parameters{};
    parameters[model::LvolStore::UUID] = uuid;
    return send_request(Method::GET_LVOL_STORES, parameters);
}


std::vector<spdk::model::LvolStore> SpdkApi::get_lvol_stores_by_name(const std::string& lvs_name) {
    json::Json parameters{};
    parameters[model::LvolStore::NAME] = lvs_name;
    return send_request(Method::GET_LVOL_STORES, parameters);
}


std::vector<spdk::model::NvmfSubsystem> SpdkApi::get_nvmf_subsystems() {
    return send_request(Method::GET_NVMF_SUBSYSTEMS);
}


std::string SpdkApi::construct_lvol_store(const std::string& bdev_name,
                                          const std::string& lvs_name,
                                          const JsonOptional<std::uint64_t>& cluster_size) {
    json::Json parameters{};
    parameters[BDEV_NAME] = bdev_name;
    parameters[LVS_NAME] = lvs_name;

    if (cluster_size) {
        parameters[CLUSTER_SIZE] = cluster_size.value();
    }

    return send_request(Method::CONSTRUCT_LVOL_STORE, parameters);
}


std::string SpdkApi::construct_lvol_bdev_by_store_uuid(const std::string& lvol_name,
                                                       const std::uint64_t size,
                                                       const std::string& uuid,
                                                       const JsonOptional<bool>& thin_provision,
                                                       const JsonOptional<std::string>& clear_method) {
    json::Json parameters{};
    parameters[LVOL_NAME] = lvol_name;
    parameters[SIZE] = size;
    parameters[UUID] = uuid;

    if (thin_provision) {
        parameters[THIN_PROVISION] = thin_provision;
    }
    if (clear_method) {
        parameters[CLEAR_METHOD] = clear_method;
    }

    return send_request(Method::CONSTRUCT_LVOL_BDEV, parameters);
}


std::string SpdkApi::construct_lvol_bdev_by_store_name(const std::string& lvol_name,
                                                       const std::uint64_t size,
                                                       const std::string& lvs_name,
                                                       const JsonOptional<bool>& thin_provision,
                                                       const JsonOptional<std::string>& clear_method) {
    json::Json parameters{};
    parameters[LVOL_NAME] = lvol_name;
    parameters[SIZE] = size;
    parameters[LVS_NAME] = lvs_name;

    if (thin_provision) {
        parameters[THIN_PROVISION] = thin_provision;
    }
    if (clear_method) {
        parameters[CLEAR_METHOD] = clear_method;
    }

    return send_request(Method::CONSTRUCT_LVOL_BDEV, parameters);
}


bool SpdkApi::nvmf_subsystem_create(const std::string& nqn, const spdk::JsonOptional<std::string>& serial_number,
                                    const spdk::JsonOptional<uint64_t>& max_namespaces,
                                    const spdk::JsonOptional<bool>& allow_any_host) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = nqn;
    parameters[model::NvmfSubsystem::ALLOW_ANY_HOST] = allow_any_host.value_or(false);
    parameters[model::NvmfSubsystem::MAX_NAMESPACES] = max_namespaces.value_or(0);
    parameters[model::NvmfSubsystem::SERIAL_NUMBER] = serial_number.value_or(DEFAULT_SERIAL_NUMBER);

    return send_request(Method::NVMF_SUBSYSTEM_CREATE, parameters);
}


bool SpdkApi::delete_nvmf_subsystem(const std::string& nqn) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = nqn;

    return send_request(Method::DELETE_NVMF_SUBSYSTEM, parameters);
}


bool SpdkApi::destroy_lvol_store(const std::string& uuid, const std::string& lvs_name) {
    json::Json parameters{};

    if (!uuid.empty()) {
        parameters[model::LvolStore::UUID] = uuid;
    }
    if (!lvs_name.empty()) {
        parameters[LVS_NAME] = lvs_name;
    }

    return send_request(Method::DESTROY_LVOL_STORE, parameters);
}


bool SpdkApi::destroy_lvol_bdev(const std::string& name) {
    json::Json parameters{};

    parameters[model::Bdev::NAME] = name;
    return send_request(Method::DESTROY_LVOL_BDEV, parameters);
}


bool SpdkApi::construct_raid_bdev(const std::string& raid_bdev_name,
                                  const std::uint64_t strip_size,
                                  const std::vector<std::string>& base_bdevs,
                                  const std::uint8_t raid_level) {

    json::Json parameters{};

    parameters[NAME] = raid_bdev_name;
    parameters[STRIP_SIZE] = strip_size;
    parameters[RAID_LEVEL] = raid_level;
    parameters[BASE_BDEVS] = base_bdevs;

    return send_request(Method::CONSTRUCT_RAID_BDEV, parameters);
}


bool SpdkApi::resize_lvol_bdev(const std::string& lvol_uuid, const std::uint64_t size) {
    json::Json parameters{};

    parameters[NAME] = lvol_uuid;
    parameters[SIZE] = size;

    return send_request(Method::RESIZE_LVOL_BDEV, parameters);
}


bool SpdkApi::destroy_raid_bdev(const std::string& raid_bdev_name) {
    json::Json parameters{};

    parameters[NAME] = raid_bdev_name;

    return send_request(Method::DESTROY_RAID_BDEV, parameters);
}


std::vector<std::string> SpdkApi::get_raid_bdevs_names(const std::string& category) {
    json::Json parameters{};

    parameters[CATEGORY] = category;

    return send_request(Method::GET_RAID_BDEVS, parameters);
}


std::string SpdkApi::snapshot_lvol_bdev(const std::string& lvol_name, const std::string& snapshot_name) {
    json::Json parameters{};

    parameters[LVOL_NAME] = lvol_name;
    parameters[SNAPSHOT_NAME] = snapshot_name;

    return send_request(Method::SNAPSHOT_LVOL_BDEV, parameters);
}


std::string SpdkApi::clone_lvol_bdev(const std::string& snapshot_name, const std::string& clone_name) {
    json::Json parameters{};

    parameters[SNAPSHOT_NAME] = snapshot_name;
    parameters[CLONE_NAME] = clone_name;

    return send_request(Method::CLONE_LVOL_BDEV, parameters);
}


bool SpdkApi::inflate_lvol_bdev(const std::string& lvol_name) {
    json::Json parameters{};
    parameters[NAME] = lvol_name;
    return send_request(Method::INFLATE_LVOL_BDEV, parameters);
}


std::uint32_t SpdkApi::nvmf_subsystem_add_ns(const std::string& subsystem_nqn, const spdk::model::Namespace& ns) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = subsystem_nqn;
    parameters[model::NvmfSubsystem::NAMESPACE] = ns;
    return send_request(Method::NVMF_SUBSYSTEM_ADD_NS, parameters);
}


bool SpdkApi::nvmf_subsystem_remove_host(const std::string& subsystem_nqn, const std::string& host_nqn) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = subsystem_nqn;
    parameters[model::NvmfSubsystem::HOST] = host_nqn;
    return send_request(Method::NVMF_SUBSYSTEM_REMOVE_HOST, parameters);
}


bool SpdkApi::nvmf_subsystem_remove_host(const std::string& subsystem_nqn, const spdk::model::Host& host) {
    return nvmf_subsystem_remove_host(subsystem_nqn, host.get_nqn());
}


bool SpdkApi::nvmf_subsystem_add_host(const std::string& subsystem_nqn, const std::string& host_nqn) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = subsystem_nqn;
    parameters[model::NvmfSubsystem::HOST] = host_nqn;
    return send_request(Method::NVMF_SUBSYSTEM_ADD_HOST, parameters);
}


bool SpdkApi::nvmf_subsystem_add_host(const std::string& subsystem_nqn, const model::Host& host) {
    return nvmf_subsystem_add_host(subsystem_nqn, host.get_nqn());
}


bool SpdkApi::nvmf_subsystem_add_listener(const std::string& subsystem_nqn,
                                          const spdk::model::ListenAddress& listen_address) {
    json::Json parameters{};
    parameters[model::NvmfSubsystem::NQN] = subsystem_nqn;
    parameters[model::NvmfSubsystem::LISTEN_ADDRESS] = listen_address;
    return send_request(Method::NVMF_SUBSYSTEM_ADD_LISTENER, parameters);
}
