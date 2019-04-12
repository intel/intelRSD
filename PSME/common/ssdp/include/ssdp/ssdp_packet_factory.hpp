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

#include "ssdp_packet.hpp"

#include <memory>

namespace ssdp {

class SsdpServiceConfig;
class SsdpPacketFactoryImpl;

/*!
 * SsdpPacket factory.
 */
class SsdpPacketFactory final {
public:
    /*!
     * Constructor
     * param[in] config SsdpServiceConfig object
     */
    SsdpPacketFactory(std::shared_ptr<const SsdpServiceConfig> config);

    /*! Destructor */
    ~SsdpPacketFactory();

    /*!
     * Creates SsdpPacket of given type
     * param[in] type Type of SsdpPacket to be created
     */
    SsdpPacket create_ssdp_packet(const SsdpPacketKey& type) const;

private:
    std::unique_ptr<SsdpPacketFactoryImpl> m_impl;
};

}
