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
 * @file spdk_agent_context.hpp
 */

#pragma once



#include "spdk/spdk_api.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"
#include "interface-reader/base_interface_reader.hpp"
#include "loader/spdk_loader.hpp"
#include "utils/transaction/base_transaction_handler_factory.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <set>



namespace agent {
namespace spdk {

/*!
 * Contains all interfaces that are required by the SPDK agent discovery.
 * All interfaces will be used from multiple threads so they have to be thread safe.
 * If this is not true for an interface, thread-safe proxy of the interface has to be provided.
 */
class SpdkAgentContext final {
public:
    using SPtr = std::shared_ptr<SpdkAgentContext>;


    /*!
     * @brief Default constructor.
     */
    SpdkAgentContext();


    /*!
     * SPDK API allows to perform CRUD operations on SPDK components
     */
    std::shared_ptr<::spdk::SpdkApi> spdk_api{};

    /*!
     * Use this mutex before any operation on SPDK API to prevent accessing SPDK during background re-discovery
     */
    std::mutex spdk_mutex{};

    /*!
     * Interface of the reader that is able to list ethernet interface.
     */
    interface_reader::BaseInterfaceReader::SPtr interface_reader{};

    /*!
     * Interface to sysfs to read system GUID
     */
    std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface{};

    /*!
     * Running spdk configuration.
     */
    std::shared_ptr<loader::SpdkConfiguration> configuration{};

    /*!
     * Factory for creating transactions.
     */
    std::shared_ptr<::utils::transaction::BaseTransactionHandlerFactory> transaction_handler_factory{};

    /*!
     * Names of SPDK volumes which are removed from API.
     */
    std::set<std::string> hidden_volumes{};

};

/*!
 * Declare context class used in the following declarations.
 * Change this declaration to change the context class used in commands.
 * */
using AgentContext = SpdkAgentContext;

}
}
