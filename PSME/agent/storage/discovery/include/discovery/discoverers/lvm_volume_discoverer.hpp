/*!
 * @brief Volume discoverer interface.
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
 * @file lvm_volume_discoverer.hpp
 */

#pragma once



#include "agent-framework/discovery/discoverers/volume_discoverer.hpp"
#include "storage_agent_context.hpp"


namespace agent {
namespace storage {
namespace discovery {

class LvmVolumeDiscoverer : public agent_framework::discovery::VolumeDiscoverer {
public:

    /*!
     * @brief Constructor
     * @param context Storage agent context pointer.
     */
    explicit LvmVolumeDiscoverer(AgentContext::SPtr context) : m_context(context) { }


    /*! @brief Virtual destructor */
    virtual ~LvmVolumeDiscoverer() { }

    /*!
     * @brief Perform discovery of volume objects.
     * @param parent_uuid Parent UUID.
     * @return Volume collection filled by the method with discovered data.
     * */
    virtual std::vector<agent_framework::model::Volume> discover(const Uuid& parent_uuid) override;


private:
    AgentContext::SPtr m_context{};

};

}
}
}