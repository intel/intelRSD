/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file storage_agent_context.hpp
 */

#pragma once



#include "drive_handling/base_drive_reader.hpp"
#include "drive_handling/base_drive_handler_factory.hpp"
#include "loader/configuration.hpp"

#include "lvm/lvm_api.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"
#include "interface-reader/base_interface_reader.hpp"


#include <memory>


namespace agent {
namespace storage {

/*!
 * Contains all interfaces that are required by the agent.
 * All interfaces will be used from multiple threads so they have to be thread safe.
 * If this is not true for an interface, thread-safe proxy of the interface has to be provided.
 */
class StorageAgentContext final {
public:
    using SPtr = std::shared_ptr<StorageAgentContext>;


    /*!
     * @brief Initialize context with agent-specific structures.
     * @param configuration_ptr Initial configuration.
     */
    void initialize(std::shared_ptr<loader::AgentConfiguration> configuration_ptr);


    /*!
     * Interface to LVM. It is thread-safe as every method has its own mutex.
     */
    std::shared_ptr<::storage::lvm::LvmApi> lvm_api{};


    /*!
     * Interface of the reader that is able to list ethernet interface.
     */
    interface_reader::BaseInterfaceReader::SPtr interface_reader{};

    /*!
     * Interface of the reader that is able to list drives.
     * It is thread safe as it is stateless and only reads data from sysfs.
     */
    BaseDriveReader::SPtr drive_reader{};

    /*!
     * Drive factory used to build drive handlers depending on drive's capabilities.
     */
    BaseDriveHandlerFactory::SPtr drive_handler_factory{};

    /*!
     * Interface to sysfs, for example to read system GUID.
     */
    std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface{};

    /*!
     * Running storage configuration.
     */
    std::shared_ptr<loader::AgentConfiguration> configuration{};

};

/*!
 * Declare context class used in the following declarations.
 * Change this declaration to change the context class used in commands.
 * */
using AgentContext = StorageAgentContext;

}
}
