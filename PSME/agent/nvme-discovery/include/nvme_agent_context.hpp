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

#include "tools/base_database_factory.hpp"
#include "tools/base_transaction_handler_factory.hpp"

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
     * Rollback factory used to build rollback handlers.
     */
    std::shared_ptr<tools::BaseTransactionHandlerFactory> transaction_handler_factory{};

    /*!
     * Database factory used to retrieve nvme related databases.
     */
    std::shared_ptr<tools::BaseDatabaseFactory> db_factory{};
};

}
}
