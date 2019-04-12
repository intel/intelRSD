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
 */

#include "agent-framework/threading/thread.hpp"

using namespace agent_framework::threading;

Thread::~Thread() { }

void Thread::start() {
    if (!is_running()) {
        set_running(true);
        m_thread = std::thread(&Thread::execute, this);
    }
}

void Thread::stop() {
    if (is_running()) {
        set_running(false);
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}
