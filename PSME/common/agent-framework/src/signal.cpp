/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * @file signal.cpp
 *
 * @brief Signal implementation
 * */

#include "agent-framework/signal.hpp"
#include "logger/logger.hpp"

#include <csignal>
#include <mutex>
#include <condition_variable>

using namespace agent_framework::generic;

static std::condition_variable* gp_cond_variable = nullptr;
static std::mutex g_mutex {};
static volatile size_t waiters = 0;

static void m_ignore(int signal) {
    log_info(LOGUSR, "Signal ignored: " << unsigned(signal));
}

static void m_interrupt_handler(int signal) {

    log_info(LOGUSR, "Signal received: " << unsigned(signal));

    std::unique_lock<std::mutex> lk(g_mutex);

    if (nullptr != gp_cond_variable) {
        gp_cond_variable->notify_all();
    }
}

void agent_framework::generic::wait_for_interrupt() {
    log_debug(LOGUSR, "Waiting for interrupt signal...");

    std::unique_lock<std::mutex> lk(g_mutex);

    /* Create new condition variable resource if not exist */
    if ((nullptr == gp_cond_variable) && (0 == waiters)) {
        gp_cond_variable = new std::condition_variable;
        signal(SIGINT, m_interrupt_handler);
        signal(SIGHUP, m_ignore);
    }

    ++waiters;

    /* Block the program and wait for interrupt */
    gp_cond_variable->wait(lk);

    --waiters;

    if (0 == waiters) {
        /* Ignore interrupt signal */
        signal(SIGINT, SIG_IGN);

        /* Delete condition variable resource */
        delete gp_cond_variable;
        gp_cond_variable = nullptr;
    }
}
