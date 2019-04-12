/*!
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file discovery_agent_context.hpp
 */

#pragma once

#include "tools/base_database_factory.hpp"


namespace agent {
namespace fpga_discovery {

/*!
 * Contains all interfaces that are required by the FPGA agent discovery.
 * All interfaces will be used from multiple threads so they have to be thread safe.
 * If this is not true for an interface, thread-safe proxy of the interface has to be provided.
 */
class DiscoveryAgentContext final {
public:

    /*!
     * Database factory used to retrieve fpga related databases.
     */
    std::shared_ptr<tools::BaseDatabaseFactory> db_factory{};
};

}
}
