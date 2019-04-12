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
 * @file ethtool.hpp
 *
 * @brief Ethtool class.
 * */

#pragma once
#include <stdint.h>
#include <string>

/*! forward declarations of types that are used */
struct ethtool_cmd;

namespace agent {
namespace network {
namespace tools {

/*!
 * @brief Ethtool class.
 */
class Ethtool {
public:
    using IfName = std::string;

    /*!
     * @enum Duplex
     * @brief Duplex mode.
     *
     * @var Duplex::HALF
     * half duplex
     *
     * @var Duplex::FULL
     * full duplex
     *
     * @var Duplex::UNKNOWN
     * unknown duplex
     * */
    enum class Duplex {
        HALF = 0,
        FULL,
        UNKNOWN
    };

    /*!
     * @brief Default constructor.
     */
    Ethtool() = delete;

    /*!
     * @brief Default constructor.
     * @param[in] port Port identifier.
     */
    Ethtool(const IfName& port);

    /*!
     * @brief Default destructor.
     */
    virtual ~Ethtool();

    /*!
     * @brief Copy constructor.
     * @param[in] orig object to be copied.
     */
    Ethtool(const Ethtool& orig) = delete;

    /*!
     * @brief Copy operator.
     * @param[in] s object to be copied
     */
    Ethtool& operator= (const Ethtool& s) = delete;

    /*!
     * @brief Set speed on interface.
     * @param[in] speed Port speed.
     */
    void set_speed(uint32_t speed);

    /*!
     * @brief Get speed on interface.
     * @return state Port speed.
     */
    uint32_t get_speed();

    /*!
     * @brief Get auto-negotiation flag on interface.
     * @return true or false.
     */
    bool get_autoneg();

    /*!
     * @brief Set auto-negotiation on interface.
     * @param[in] enable Port auto-negotiation.
     */
    void set_autoneg(bool enable);

    /*!
     * @brief Get full duplex flag on interface.
     * @return true or false.
     */
    Duplex get_duplex();

private:
    /*!
     * @brief Get ethtool settings.
     * @param[out] edata Ethtool data.
     */
    void get_settings(struct ethtool_cmd& edata);

    /*!
     * @brief Set ethtool settings.
     * @param[in,out] edata Ethtool data.
     */
    void set_settings(struct ethtool_cmd& edata);

    /*!
     * @brief Network socket.
     */
    int m_sock;

    /*!
     * @brief Interface name.
     */
    std::string m_ifname;
};

}
}
}

