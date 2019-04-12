/*!
 * @brief Fabric builder class interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file fabric_builder.hpp
 */

#pragma once

#include "agent-framework/module/model/fabric.hpp"


namespace agent_framework {
namespace discovery {

class FabricBuilder {
public:

    /*!
     * @brief Build default fabric object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default fabric object.
     */
    static agent_framework::model::Fabric build_default(const Uuid& parent_uuid);

    /*!
     * @brief Updates fabric model with iSCSI protocol type.
     * @param[in,out] fabric Fabric to be updated.
     */
    static void iscsi_protocol(agent_framework::model::Fabric& fabric);


    /*!
     * @brief Updates fabric model with NVMe-oF protocol type.
     * @param[in,out] fabric Fabric to be updated.
     */
    static void nvmeof_protocol(agent_framework::model::Fabric& fabric);

    /*!
     * @brief Updates fabric model with FPGA-oF protocol type.
     * @param[in,out] fabric Fabric to be updated.
     */
    static void fpgaof_protocol(agent_framework::model::Fabric& fabric);
};

}
}

