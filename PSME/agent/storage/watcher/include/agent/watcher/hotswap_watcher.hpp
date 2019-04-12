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
#include "agent/watcher/hotswap_manager.hpp"



namespace agent {
namespace storage {
namespace watcher {

/*!
 * @brief Drives HotSwap watcher
 */
class HotswapWatcher final : public agent_framework::threading::Thread {
public:

    /*!
     * @brief Default constructor
     * @param context Agent context shared pointer.
     * */
    HotswapWatcher(AgentContext::SPtr context) : m_context(context) {}


    /*! @brief Hotswap watcher loop */
    void execute() override;


    /*! @brief Default destructor */
    ~HotswapWatcher();


private:

    AgentContext::SPtr m_context{};

    /*! @brief Start discovery after hotswap. */
    void handle_hotswap();


    /*!
     * @brief Detects hotswap of the hard drives.
     * @param[in] fw INotify instance handle.
     * @param[in] wd Hard drives directory handle.
     * @return True if hotswap was detected.
     */
    bool detect_hotswap(int fw, int wd);
};

}
}
}
