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
 * @file ssdp_packet_validator.hpp
 *
 * @brief Declaration of SsdpPacketValidator.
 * */

#pragma once

#include <string>
#include <memory>

namespace ssdp {

class SsdpPacket;

/*!
 * SSDP packet validator interface.
 */
class SsdpPacketValidator {
public:
    using Ptr = std::unique_ptr<SsdpPacketValidator>;

    /*! Destructor */
    virtual ~SsdpPacketValidator();

    /*!
     * Checks validity of SSDP packet.
     * Verifies all required headers are present.
     * @return true if SsdpPacket is valid, false otherwise.
     */
    virtual bool is_valid(const SsdpPacket& packet) const = 0;
};

/*!
 * SSDP notify packet validator.
 */
class SsdpNotifyPacketValidator : public SsdpPacketValidator {
public:
    bool is_valid(const SsdpPacket& packet) const override;

protected:
    virtual bool validate_subtype(const SsdpPacket& packet) const = 0;
};

/*!
 * SSDP alive packet validator.
 */
class SsdpAlivePacketValidator : public SsdpNotifyPacketValidator {
protected:
    bool validate_subtype(const SsdpPacket& packet) const override;
};

/*!
 * SSDP byebye packet validator.
 */
class SsdpByeByePacketValidator : public SsdpNotifyPacketValidator {
protected:
    bool validate_subtype(const SsdpPacket& packet) const override;
};

/*!
 * SSDP search request packet validator.
 */
class SsdpSearchRequestPacketValidator : public SsdpPacketValidator {
public:
    bool is_valid(const SsdpPacket& packet) const override;
};

/*!
 * SSDP search response packet validator.
 */
class SsdpSearchResponsePacketValidator : public SsdpPacketValidator {
public:
    bool is_valid(const SsdpPacket& packet) const override;
};

/*!
 * Checks if all required headers are present.
 * @param[in] packet SsdpPacket to be validated.
 * @return true if SsdpPacket is valid, false otherwise.
 */
bool validate_ssdp_packet(const SsdpPacket& packet);

}
