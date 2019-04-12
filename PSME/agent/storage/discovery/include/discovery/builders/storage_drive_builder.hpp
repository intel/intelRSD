/*!
 * @brief Drive builder class interface.
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
 * @file storage_drive_builder.hpp
 */

#pragma once



#include "agent-framework/discovery/builders/drive_builder.hpp"
#include "drive_handling/base_drive_handler.hpp"


namespace agent {
namespace storage {
namespace discovery {

class StorageDriveBuilder : public agent_framework::discovery::DriveBuilder {
public:

    /*!
     * @brief Updates drive model with drive handler data.
     * @param drive Drive to be updated.
     * @param drive_data Data from drive handler.
     */
    static void update_with_drive_handler(agent_framework::model::Drive& drive,
                                          const agent::storage::BaseDriveHandler::DriveData& drive_data);

};

}
}
}

