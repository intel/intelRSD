/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file port_builder.hpp
 *
 * @brief PortBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "cable_id.hpp"
#include "gas/csr/configuration_space_register.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/top/top_level_registers.hpp"
#include "tools/model_tool.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create port objects
 */
class PortBuilder : public AbstractBuilder<agent_framework::model::Port> {

public:

    using ReturnType = PortBuilder&;

    PortBuilder() = default;
    ~PortBuilder() = default;

    /*!
     * @brief Updates cable id data of the port
     * @param[in] cable_id Read cable id data structure
     * @return Pointer to the builder
     * */
    virtual ReturnType update_cable_id(const CableId& cable_id);

    /*!
     * @brief Updates binding info for ports
     * @param[in] entry_id Id of the entry in the Port Binding Info describing the port
     * @param[in] cmd Result of the port binding info command
     * @param[in] top Read top level registers data
     * @return Pointer to the builder
     * */
    virtual ReturnType update_binding_info(const std::uint32_t entry_id, const gas::mrpc::PortBindingInfo& cmd,
        const gas::top::TopLevelRegisters& top);

    /*!
     * @brief Updates twi config data based on the provided data
     * @param[in] twi_port Twi port
     * @param[in] twi_channel Twi_channel
     * @return Pointer to the builder
     * */
    virtual ReturnType update_twi_config(uint32_t twi_port, uint32_t twi_channel);

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;

};

}
}
}
}
