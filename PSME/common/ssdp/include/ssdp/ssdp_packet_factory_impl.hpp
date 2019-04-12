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
 * @file ssdp_packet_factory.hpp
 *
 * @brief Declaration of SsdpPacketFactory.
 * */

#pragma once

#include "ssdp_packet_factory.hpp"

#include <memory>
#include <unordered_map>

namespace ssdp {

class SsdpServiceConfig;

/*!
 * SsdpPacket factory.
 */
class SsdpPacketFactoryImpl final {
public:
    /*!
     * Constructor
     * param[in] config SsdpServiceConfig object
     */
    SsdpPacketFactoryImpl(std::shared_ptr<const SsdpServiceConfig> config);

    SsdpPacket create_ssdp_packet(const SsdpPacketKey& type) const;

private:
    SsdpPacket create_ssdp_alive() const;
    SsdpPacket create_ssdp_search_reply() const;
    SsdpPacket create_ssdp_byebye() const;
    SsdpPacket create_ssdp_search_request() const;

    std::shared_ptr<const SsdpServiceConfig> m_config;
    std::unordered_map<SsdpPacketKey, SsdpPacket> m_prototypes{};
};

}
