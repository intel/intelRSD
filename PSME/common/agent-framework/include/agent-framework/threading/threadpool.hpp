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
 *
 *
 * @file threadpool.hpp
 *
 * @brief Threadpool interface
 * */

#pragma once
#include "thread_queue.hpp"
#include <thread>
#include <functional>
#include <future>
#include <vector>

namespace agent_framework {
namespace threading {

/*! Threadpool task implementation */
class Task {
    /*! Base class for function wrapper */
    struct Base {
        virtual ~Base();
        /*! Class function interface */
        virtual void call() = 0;
    };

    /*!
     * @brief Function wrapper implementation
     *
     * @tparam F Function parameter
     */
    template<typename F>
    struct Wrapper final : public Base {
        Wrapper(F&& f) : m_f(std::forward<F>(f)) {}
        void call() final override { m_f(); }
        F m_f;
    };

public:
    enum class Type {
        STOP,
        REGULAR
    };

    /*!
     * @brief Task default constructor default create task stop type
     */
    Task() : m_type{Type::STOP}, m_impl{nullptr} {}

    template<typename F>
    Task(F&& f) :
        m_type{Type::REGULAR},
        m_impl{new Wrapper<F>(std::forward<F>(f))} {}

    /*! Disable copy */
    Task(const Task&) = delete;
    Task(Task&) = delete;
    Task& operator=(const Task&) = delete;

    /*! Enable move */
    Task(Task&& other) = default;
    Task& operator=(Task&& other) = default;

    void operator()() {
        m_impl->call();
    }

    bool is_stop() const { return Type::STOP == m_type; }

private:
    Type m_type;
    std::unique_ptr<Base> m_impl;
};

/*! Threadpool implementation */
class Threadpool
{
public:
    /*! Create threadpool object with given numbers of thread */
    explicit Threadpool(const std::size_t thread_count =
                                        std::thread::hardware_concurrency());
    ~Threadpool();

    /*! Disable copy */
    Threadpool(const Threadpool&) = delete;
    Threadpool& operator=(const Threadpool&) = delete;

    /*!
     * @brief Run function or callable object in threadpool
     * @param f Function or callable object
     * @param args Arguments
     */
    template<typename F, typename... Args>
    void run(F&& f, Args&&... args) {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        m_tasks.push_back(std::move(task));
    }

    /*! Stop threadpool */
    void stop(bool force);

private:
    void create_threads();
    void join_threads();
    void stop_threads();
    void run_loop();

    using Threads = std::vector<std::thread>;
    using Tasks = ThreadQueue<Task>;

    std::size_t m_thread_count;
    Threads m_threads;
    Tasks m_tasks;

    std::atomic<unsigned> m_running_tasks{};
};

}
}
