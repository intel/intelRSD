/*!
 * @brief Implementation of SpdkConverter.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file spdk_converter.cpp
 */

#include "spdk_converter.hpp"



using namespace ::spdk::model;
using namespace agent::spdk::utils;
using namespace agent_framework::model;

constexpr const char agent::spdk::utils::SpdkConverter::RSD_SERIAL_NUMBER[];


Namespace SpdkConverter::convert_volume_to_namespace(const Volume& volume) {
    auto storage_pools = agent_framework::module::get_m2m_manager<StoragePool, Volume>().get_parents(volume.get_uuid());
    if (storage_pools.empty()) {
        throw std::runtime_error("No storage pool assigned for volume " + volume.get_name().value());
    }

    Namespace nvmf_namespace{};
    auto storage_pool = agent_framework::module::get_manager<StoragePool>().get_entry(storage_pools.front());
    auto alias = SpdkConverter::make_alias(storage_pool.get_name(), attribute::Identifier::get_uuid(volume));
    nvmf_namespace.set_bdev_name(alias);
    return nvmf_namespace;
}


Host SpdkConverter::convert_initiator_to_host(const Endpoint& endpoint) {
    Host host{};
    host.set_nqn(attribute::Identifier::get_nqn(endpoint));
    return host;
}

ListenAddress SpdkConverter::convert_to_listen_address(
    const attribute::IpTransportDetail& ip_transport_detail) {

    ListenAddress address{};
    address.set_trtype(TransportType::RDMA);
    address.set_adrfam(AddressFamily::IPv4);
    address.set_traddr(ip_transport_detail.get_ipv4_address().get_address());
    address.set_trsvcid(std::to_string(ip_transport_detail.get_port()));

    return address;
}

std::string SpdkConverter::make_alias(const std::string& lvs_name, const std::string& lvol_bdev) {
    return lvs_name + "/" + lvol_bdev;
}