/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "agent-framework/threading/thread.hpp"
#include "storage_agent_context.hpp"


namespace agent {
namespace storage {
namespace watcher {

/*!
 * @brief Ip watcher
 */
class IpWatcher final : public agent_framework::threading::Thread {
public:

    /*!
     * @brief Constructor
     * @param context Shared pointer to Agent context
     * */
    IpWatcher(AgentContext::SPtr context) : m_context(context) {}


    /*! @brief IP watcher loop */
    void execute() override;


    /*! @brief Default destructor */
    ~IpWatcher();


private:
    AgentContext::SPtr m_context{};

};

}
}
}
