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
 * @file zone_builder.hpp
 *
 * @brief ZoneBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "gas/partition/partition_configuration.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create zone objects
 */
class ZoneBuilder : public AbstractBuilder<agent_framework::model::Zone> {

public:

    using ReturnType = ZoneBuilder&;

    ZoneBuilder() = default;
    ~ZoneBuilder() = default;

    /*!
     * @brief Updates partition configuration data
     * @param[in] pc Read partition configuration data
     * @return Pointer to the builder
     * */
    virtual ReturnType update_pc(const gas::partition::PartitionConfiguration& pc);

    /*!
     * @brief Updates zone id
     * @param[in] zone_id Id of the zone as in GAS
     * @return Pointer to the builder
     * */
    virtual ReturnType update_id(std::uint8_t zone_id);

    /*!
     * @brief Updates links to other resources
     * @param[in] switch_uuid Switch uuid
     * @return Pointer to the builder
     * */
    virtual ReturnType update_links(const std::string& switch_uuid);

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
