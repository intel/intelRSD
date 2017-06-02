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
 *
 * @brief Declaration of WorkerThread
 * */

#pragma once

#include "threadsafe_queue.hpp"

#include <functional>
#include <thread>
#include <future>

namespace generic {

template<class RetType, class Fun, class... Args>
void set_promise_value(std::promise<RetType>& prom, Fun& fun, Args&&... args ) {
    prom.set_value(fun(std::forward<Args>(args)... ));
}

template<class Fun, class... Args>
void set_promise_value(std::promise< void > & prom, Fun & fun, Args&&... args ) {
    fun(std::forward<Args>(args)...);
    prom.set_value();
}


class WorkerThread final {
public:
    WorkerThread() : m_worker {
        [this] {
            while (!m_stop) {
                m_tasks.wait_and_pop()->operator()();
            }
        }
    } {}

    template<class Fun, class... Args>
    auto operator()(Fun fun, Args... args) const -> std::future<decltype(fun(args...))> {
        std::function<decltype(fun(args...))()> f = std::bind(fun, args...);
        auto prom = std::make_shared<std::promise<decltype(fun(args...))>>();
        auto fut = prom->get_future();
        m_tasks.push_back([=]{
            try {
                set_promise_value(*prom, f);
            }
            catch (...) {
                prom->set_exception(std::current_exception());
            }
        });
        return fut;
    }

    ~WorkerThread() {
        m_tasks.push_back([this] { m_stop = true; });
        m_worker.join();
    }

private:
    mutable ThreadSafeQueue<std::function<void()>> m_tasks{};
    std::thread m_worker;
    bool m_stop{false};
};

}
