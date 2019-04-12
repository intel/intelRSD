/*!
 * @brief Class to keep ipmi bridging information
 *
 * Simple container, not to be overriden.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/ipmi_bridge.hpp
 */

#pragma once

#include <cstdint>
#include <ostream>

namespace ipmi {

class BridgeInfo final {
public:
    using Channel = std::uint8_t;
    using Address = std::uint32_t;

    enum class Level : std::uint8_t {
        DIRECT = 0,
        SINGLE_BRIDGE,
        DUAL_BRIDGE
    };


    /*!
     * @brief Construct info without bridging
     */
    BridgeInfo();


    /*!
     * @brief Construct info with direct bridging (single bridge)
     */
    BridgeInfo(Address target_address, Channel target_channel);


    /*!
     * @brief Construct info with transit bridging (dual bridge)
     */
    BridgeInfo(Address transit_address, Channel transit_channel, Address target_address, Channel target_channel);


    /*!
     * @brief Non-virtual destructor
     */
    ~BridgeInfo();

    /*!
     * @brief Default constructor
     */
    BridgeInfo(const BridgeInfo&) = default;


    /*!
     * @brief Default constructor
     */
    BridgeInfo(BridgeInfo&&) = default;


    /*!
     * @brief Get bridging level (none/target only/dual bridge)
     * @return Bridging level
     */
    Level get_level() const {
        return level;
    }


    /*!
     * @brief Get bridge params for target
     * @param channel channel to be set
     * @param address address to be set
     * @return true if (any) bridging is defined
     */
    bool get_transit(Address& address, Channel& channel) const;


    /*!
     * @brief Get bridge params for target
     * @param channel reference to be filled
     * @param address reference to be filled
     * @return true if (any) bridging is defined
     */
    bool get_target(Address& address, Channel& channel) const;


    /*!
     * @brief Set bridge params for transit. Bridge must be set correctly,
     *        set doesn't alter bridging level.
     * @param channel channel to be set
     * @param address address to be set
     * @return true if (any) bridging is defined
     */
    bool set_transit(Address address, Channel channel);


    /*!
     * @brief Set transit channel. Bridge must be set correctly,
     *        set doesn't alter bridging level.
     * @param channel channel to be set
     * @return true if (any) bridging is defined
     */
    bool set_transit(Channel channel);


    /*!
     * @brief Set bridge params for target. Bridge must be set correctly,
     *        set doesn't alter bridging level.
     * @param channel reference to be filled
     * @param address reference to be filled
     * @return true if (any) bridging is defined
     */
    bool set_target(Address address, Channel channel);


    /*!
     * @brief Set target channel. Bridge must be set correctly,
     *        set doesn't alter bridging level.
     * @param channel channel to be set
     * @return true if (any) bridging is defined
     */
    bool set_target(Channel channel);


private:
    const Level level;
    Address transit_address{};
    Channel transit_channel{};
    Address target_address{};
    Channel target_channel{};
};

}

std::ostream& operator<<(std::ostream& ostr, const ipmi::BridgeInfo& info);
