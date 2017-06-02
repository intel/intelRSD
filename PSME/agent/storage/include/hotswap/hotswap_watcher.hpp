
/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "hotswap/hotswap_manager.hpp"

namespace agent {
namespace storage {

/*!
 * @brief HotSwap watcher
 */
class HotswapWatcher final : public agent_framework::threading::Thread {
public:
    /*! @brief Default constructor */
    HotswapWatcher() = default;

    /*! @brief Hotswap watcher loop */
    void execute() override;

    /*! @brief Default destructor */
    ~HotswapWatcher();

private:
    void handle_hotswap();
    bool detect_hotswap(int, int);
};

}
}
