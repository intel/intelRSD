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
 * @file pcie_function_builder.hpp
 *
 * @brief PcieFunctionBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "sysfs/sysfs_dto.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create PcieFunction objects
 */
class PcieFunctionBuilder : public AbstractBuilder<agent_framework::model::PcieFunction> {

public:

    PcieFunctionBuilder() = default;
    ~PcieFunctionBuilder() = default;

    using ReturnType = PcieFunctionBuilder&;

    /*!
     * @brief Updates sysfs data of the function
     * @param[in] sysfs_function Function data read from sysfs
     * @return Pointer to the builder
     * */
    virtual ReturnType update_sysfs(const sysfs::SysfsFunction& sysfs_function);

    /*!
     * @brief Updates links to other resources
     * @param[in] dsp_port_uuid Dsp port uuid
     * @return Pointer to the builder
     * */
    virtual ReturnType update_links(const std::string& dsp_port_uuid);

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
