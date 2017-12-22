/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file switch_info_impl.hpp
 *
 * @brief Switch parameters
 * */

#pragma once
#include "hal/switch_info.hpp"

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch Info.
 */
class SwitchInfoImpl : public SwitchInfo {
public:
    /*!
     * @brief Default constructor.
     * @param[in] mgmt_port Management port.
     */
    SwitchInfoImpl(const std::string& mgmt_port);

    /*!
     * @brief Copy constructor.
     * @param[in] orig object to be copied.
     */
    SwitchInfoImpl(const SwitchInfoImpl& orig) = delete;

    /*!
     * @brief Copy operator
     * @param[in] orig object to be copied.
     */
    SwitchInfoImpl& operator=(const SwitchInfoImpl &orig) = delete;

    /*!
     * @brief Default destructor.
     */
    virtual ~SwitchInfoImpl();

    /*!
     * @brief Reads information about switch from the HW.
     */
    void read_switch_info();

    /*!
     * @brief Read switch port from the HW.
     */
    void read_switch_port_list();

    /*!
     * @enum PortMode
     * @brief Port interface mode.
     *
     * @var PortMode SwitchInfoImpl::LINK_AGGREGATION_STATIC
     * Link aggregation static mode (loadbalance).
     *
     * @var PortMode SwitchInfoImpl::LINK_AGGREGATION_DYNAMIC
     * Link aggregation dynamic mode (lacp).
     *
     */
    enum class PortMode {
        LINK_AGGREGATION_STATIC,
        LINK_AGGREGATION_DYNAMIC
    };

    /*! brief Logical port type command string. */
    static const constexpr char* STR_LINK_PORT_TYPE_LINK_AGGREGATION = "LinkAggregation";

    /*!
     * @brief Add switch port interface.
     * @param[in] port Port interface to be created.
     * @param[in] mode Mode of the port interface.
     */
    void add_switch_port(const PortIdentifier& port, PortMode mode);

    /*!
     * @brief Delete switch port interface.
     * @param[in] port Port interface to be deleted.
     */
    void delete_switch_port(const PortIdentifier& port);
};

}
}
}
