/*!
 * @brief Bmc class declaration.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file agent-framework/state_machine/bmc.hpp
 */

#pragma once

#include "agent-framework/state_machine/bmc_state_machine.hpp"
#include "agent-framework/module/model/attributes/connection_data.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

#include "generic/worker_thread.hpp"


namespace agent_framework {

/*!
 * @brief Base class representing BMC.
 *
 * Monitors BMC's state and executes actions on state transitions.
 * Contains worker thread for scheduling tasks targeting Bmc.
 */
class Bmc : public state_machine::BmcStateMachine {
public:
    using ConnectionData = agent_framework::model::attribute::ConnectionData;
    using Ptr = std::unique_ptr<Bmc>;
    using Duration = ::generic::WorkerThread::Duration;

    Bmc(const ConnectionData& conn,
        Bmc::Duration state_update_interval,
        ReadPresenceFn read_presence = always_false,
        ReadOnlineStatusFn read_online_state = always_false);

    ~Bmc() override;

    /*!
     * @brief Starts bmc monitoring tasks
     */
    void start();

    /*!
     * @brief Stops bmc monitoring
     */
    void stop();

    const Bmc::Duration& get_update_interval() const {
        return m_state_update_interval;
    }

    std::string get_manager_uuid() const {
        return m_manager_uuid;
    }

    void set_manager_uuid(const std::string& uuid) {
        m_manager_uuid = uuid;
    }

    const ConnectionData get_connection_data() const {
        return m_connection_data;
    }

    virtual OptionalField<std::uint16_t> get_platform_id() const = 0;

    /*!
     * @brief Post task  on worker's task queue.
     * @param fun task to be executed once
     * @param args additional arguments for the task
     */
    template<class Fun, class... Args>
    void operator()(Fun fun, Args&&... args) {
        operator()(Duration::zero(), fun, std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Posts task for delayed execution on worker's task queue.
     * @param delay initial delay for first task execution
     * @param fun task to be executed once
     * @param args additional arguments for the task
     */
    template<class Fun, class... Args>
    void operator()(Duration delay, Fun fun, Args&&... args) {
        m_worker(delay, fun, std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Posts periodic task on worker's task queue.
     * @param name task name
     * @param initial_delay Initial delay for first task execution
     * @param period Interval between task executions (counted from starting time of previous task execution)
     * @param fun task to be executed periodically
     * @param args additional arguments for the task
     */
    template <class Fun, class... Args>
    void periodic_task(const std::string& name, Duration initial_delay, Duration period, Fun fun, Args&&... args) {
        m_worker.fixed_delay_task(name, initial_delay, period, fun, std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Posts periodic task on worker's task queue.
     * @param name task name
     * @param fun task to be executed periodically (with the period basing on taks return value)
     * @param args additional arguments for the task
     */
    template <class Fun, class... Args>
    void self_scheduling_task(const std::string& name, Fun fun, Args&&... args) {
        m_worker.self_scheduling_task(name, Duration::zero(), fun, std::forward<Args&&>(args)...);
    }

protected:
    /*!
     * @brief Removes all pending tasks and schedules periodic update_state task
     */
    virtual void reset_tasks();

private:

    ConnectionData m_connection_data;
    Bmc::Duration m_state_update_interval;
    std::string m_manager_uuid{};
    ::generic::WorkerThread m_worker;
};

using BmcCollection = std::vector<Bmc::Ptr>;

} // agent_framework
