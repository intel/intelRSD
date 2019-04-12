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
 * @file ssdp_packet_factory.cpp
 *
 * @brief Definition of SsdpPacketFactory.
 * */

#include "ssdp/ssdp_packet_factory_impl.hpp"
#include "ssdp/ssdp_service_config.hpp"
#include "ssdp/ssdp_service.hpp"

using namespace ssdp;

namespace {

std::string build_usn(const SsdpServiceConfig& config) {
    return "uuid:" + config.get_service_uuid() + "::" + config.get_service_urn();
}

std::string build_cache_control(const SsdpServiceConfig& config) {
    return "max-age=" + std::to_string(config.get_announce_interval().count() << 1);
}

}

SsdpPacketFactoryImpl::SsdpPacketFactoryImpl(std::shared_ptr<const SsdpServiceConfig> config)
    : m_config(config) {
    m_prototypes.emplace(SsdpPacket::Type::NOTIFY, create_ssdp_alive());
    m_prototypes.emplace(SsdpPacketKey{SsdpPacket::Type::NOTIFY, SsdpPacket::SSDP_ALIVE_STR},
            create_ssdp_alive());
    m_prototypes.emplace(SsdpPacketKey{SsdpPacket::Type::NOTIFY, SsdpPacket::SSDP_BYEBYE_STR},
            create_ssdp_byebye());
    m_prototypes.emplace(SsdpPacket::Type::SEARCH_REQUEST, create_ssdp_search_request());
    m_prototypes.emplace(SsdpPacket::Type::SEARCH_RESPONSE, create_ssdp_search_reply());
}

SsdpPacket SsdpPacketFactoryImpl::create_ssdp_packet(const SsdpPacketKey& type) const {
    return m_prototypes.at(type);
}

SsdpPacket SsdpPacketFactoryImpl::create_ssdp_alive() const {
    SsdpPacket msg(SsdpPacket::Type::NOTIFY);
    msg.set_header(SsdpPacket::HOST, SsdpService::SSDP_MCAST_ADDRESS.to_string());
    msg.set_header(SsdpPacket::CACHE_CONTROL, build_cache_control(*m_config));
    msg.set_header(SsdpPacket::NT, m_config->get_service_urn());
    msg.set_header(SsdpPacket::NTS, SsdpPacket::SSDP_ALIVE_STR);
    msg.set_header(SsdpPacket::USN, build_usn(*m_config));
    msg.set_header(SsdpPacket::AL, m_config->get_service_url());
    msg.set_header(SsdpPacket::EXT, "");
    return msg;
}

SsdpPacket SsdpPacketFactoryImpl::create_ssdp_search_reply() const {
    SsdpPacket msg(SsdpPacket::Type::SEARCH_RESPONSE);
    msg.set_header(SsdpPacket::CACHE_CONTROL, build_cache_control(*m_config));
    msg.set_header(SsdpPacket::ST, m_config->get_service_urn());
    msg.set_header(SsdpPacket::USN, build_usn(*m_config));
    msg.set_header(SsdpPacket::AL, m_config->get_service_url());
    msg.set_header(SsdpPacket::EXT, "");
    return msg;
}

SsdpPacket SsdpPacketFactoryImpl::create_ssdp_byebye() const {
    SsdpPacket msg(SsdpPacket::Type::NOTIFY);
    msg.set_header(SsdpPacket::HOST, SsdpService::SSDP_MCAST_ADDRESS.to_string());
    msg.set_header(SsdpPacket::NT, m_config->get_service_urn());
    msg.set_header(SsdpPacket::NTS, SsdpPacket::SSDP_BYEBYE_STR);
    msg.set_header(SsdpPacket::USN, build_usn(*m_config));
    return msg;
}

SsdpPacket SsdpPacketFactoryImpl::create_ssdp_search_request() const {
    SsdpPacket msg(SsdpPacket::Type::SEARCH_REQUEST);
    msg.set_header(SsdpPacket::HOST, SsdpService::SSDP_MCAST_ADDRESS.to_string());
    msg.set_header(SsdpPacket::ST, m_config->get_service_urn());
    msg.set_header(SsdpPacket::MAN, SsdpPacket::SSDP_DISCOVER);
    msg.set_header(SsdpPacket::MX, "3");
    return msg;
}
