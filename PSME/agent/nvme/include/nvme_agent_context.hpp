/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file nvme_agent_context.hpp
 */

#pragma once

#include "tools/drive_discovery/base_drive_handler_factory.hpp"
#include "tools/drive_discovery/base_drive_reader.hpp"
#include "tools/base_database_factory.hpp"
#include "nvme/abstract_nvme_interface.hpp"
#include "nvme-target/base_nvme_target_handler.hpp"
#include "partition/abstract_drive_interface.hpp"
#include "utils/transaction/base_transaction_handler_factory.hpp"

namespace agent {
namespace nvme {

/*!
 * Contains all interfaces that are required by the NVMe agent discovery.
 * All interfaces will be used from multiple threads so they have to be thread safe.
 * If this is not true for an interface, thread-safe proxy of the interface has to be provided.
 */
class NvmeAgentContext final {
public:

    /*!
     * Interface for querying NVMe commands.
     * All commands are atomic (i.e. each command is a single write operation). NVMe driver creates queues
     * for incoming requests so the interface may be used by different threads simultaneously.
     */
    std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface{};

    /*!
     * Interface for reading/writing raw drive devices.
     * There is a thread-safe proxy for drive interfaces that should be used here.
     */
    std::shared_ptr<::partition::AbstractDriveInterface> drive_interface{};

    /*!
     * Interface of the reader that is able to list drives.
     * It is thread safe as it is stateless and only reads data from sysfs.
     */
    std::shared_ptr<tools::BaseDriveReader> drive_reader{};

    /*!
     * Drive factory used to build drive handlers depending on drive's capabilities.
     */
    std::shared_ptr<tools::BaseDriveHandlerFactory> drive_handler_factory{};

    /*!
     * Interface to the NVME target configuration handler.
     * The implementation is stateless.
     */
    std::shared_ptr<nvme_target::BaseNvmeTargetHandler> nvme_target_handler{};

    /*!
     * Rollback factory used to build rollback handlers.
     */
    std::shared_ptr<::utils::transaction::BaseTransactionHandlerFactory> transaction_handler_factory{};

    /*!
     * Database factory used to retrieve nvme related databases.
     */
    std::shared_ptr<tools::BaseDatabaseFactory> db_factory{};
};

}
}
