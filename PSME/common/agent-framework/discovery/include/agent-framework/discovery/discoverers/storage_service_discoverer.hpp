/*!
 * @brief Storage service discoverer interface.
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
 * @file storage_service_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/model/storage_service.hpp"



namespace agent_framework {
namespace discovery {

class StorageServiceDiscoverer {
public:

    /*! Virtual destructor */
    virtual ~StorageServiceDiscoverer() {}


    /*!
     * @brief Perform discovery of storage service object.
     * @param parent_uuid Parent UUID.
     * @return Storage service object filled with discovered data.
     * */
    virtual model::StorageService discover(const Uuid& parent_uuid);
};

}
}