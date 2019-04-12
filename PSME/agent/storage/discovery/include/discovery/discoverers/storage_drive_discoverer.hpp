/*!
 * @brief Storage drive discoverer interface.
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
 * @file storage_drive_discoverer.hpp
 */

#pragma once



#include "storage_agent_context.hpp"
#include "agent-framework/discovery/discoverers/drive_discoverer.hpp"


namespace agent {
namespace storage {
namespace discovery {

class StorageDriveDiscoverer : public agent_framework::discovery::DriveDiscoverer {
public:

    /*!
     * @brief Constructor.
     * @param context Storage agent context pointer.
     */
    explicit StorageDriveDiscoverer(AgentContext::SPtr context) : m_context(context) { }

    /*! Virtual destructor */
    virtual ~StorageDriveDiscoverer() { }

    /*!
     * @brief Perform discovery of drive objects.
     * @param parent_uuid Parent UUID.
     * @return Drive collection filled by the method with discovered data.
     * */
    virtual std::vector<agent_framework::model::Drive> discover(const Uuid& parent_uuid);


protected:
    AgentContext::SPtr m_context{};

};

}
}
}