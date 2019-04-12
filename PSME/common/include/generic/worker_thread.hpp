/*!
 * @brief WorkerThread.
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
 * @file generic/worker_thread.hpp
 */

#pragma once

#include "logger/logger_factory.hpp"
#include "delay_queue.hpp"
#include "schedule_policy.hpp"
#include <atomic>
#include <functional>
#include <thread>

namespace generic {

/*!
 * @brief WorkerThread which can execute arbitrary tasks.
 *
 * Tasks are prioritized by expected time of execution.
 * There is also some support for periodic task execution.
 */
class WorkerThread final {
public:
    using Clock = std::chrono::steady_clock;
    using Timepoint = Clock::time_point;
    using Duration = typename Timepoint::duration;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

    class StopProcessing final : public std::runtime_error {
    public:
        StopProcessing() : std::runtime_error("Task::stopped") {}
    };

#ifdef __clang__
#pragma clang diagnostic pop
#endif

    /*!
     * @brief Constructor
     * @param name Worker's name
     */
    WorkerThread(const std::string& name = "Worker") : m_name(name), m_worker {
        [this] {
            log_info("worker", m_name << " started.");
            while (!m_stop.load(std::memory_order_acquire)) {
                auto task = m_tasks.wait_and_pop();
                try {
                    Duration duration = task();
                    if (task.is_periodic()) {
                        task.set_delay(duration);
                        m_tasks.push(std::move(task));
                    }
                }
                catch (const StopProcessing&) {
                    log_info("worker", task.get_name() << " requests to stop.");
                }
            }
            log_info("worker", m_name << " finished.");
        }
    } {}

    /*!
     * @brief Destructor
     */
    ~WorkerThread() {
        try {
            stop();
        }
        catch (...) { /* ignore */ }
    }

    /*!
     * @brief Stops worker thread.
     *
     * Any not processed tasks are discarded.
     * If called from task running on worker thread throws StopProcessing.
     */
    void stop() {
        if (std::this_thread::get_id() == m_worker.get_id()) {
            throw StopProcessing();
        }
        bool expected{false};
        if (m_stop.compare_exchange_strong(expected, true)) {
            log_debug("worker", m_name << " stopping.");
            /* If no task is currently running, the queue must be woken up. */
            high_priority_task("stop", [] () noexcept {});
            if (m_worker.joinable()) {
                m_worker.join();
            }
        }
    }

    /*!
     * @brief Checks if worker thread is running
     * @return true if worker thread is running, false otherwise
     */
    bool is_running() const {
        return !m_stop;
    }

    /*!
     * @brief Removes pending tasks from the queue
     */
    void remove_tasks() {
        if (is_running()) {
            m_tasks.clear();
        }
    }

    /*!
     * @brief Posts task on queue.
     * @param task Task to be executed
     * @param args Task arguments
     */
    template<class T, class... Args>
    void operator()(T&& task, Args&&... args) const {
        operator()(Duration::zero(), std::forward<T&&>(task), std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Posts task on queue to be invoked with delay
     * @param delay Duration of delay
     * @param task Task to be executed
     * @param args Task arguments
     */
    template<typename T, typename... Args>
    void operator()(Duration delay, T&& task, Args&&... args) const {
        single_run_task("", delay, std::forward<T&&>(task), std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Add fixed delay periodic task.
     *
     * Worker will execute the task with a fixed period between the end of the last invocation and the start of the next.
     *
     * @param name task name
     * @param initial_delay Initial delay of first execution of task
     * @param period Interval between task executions (counted from end time of previous task execution)
     * @param task Function to be executed
     * @param args Task arguments
     */
    template <class T, class... Args>
    void fixed_delay_task(const std::string& name, Duration initial_delay, Duration period,
                       T&& task, Args&&... args) const {
        periodic_task<FixedDelaySchedulePolicy<Clock>>(name, initial_delay, period,
                std::forward<T&&>(task), std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Add fixed rate periodic task.
     *
     * Worker will execute the task with a fixed period between invocations.
     *
     * @param name task name
     * @param initial_delay Initial delay of first execution of task
     * @param period Interval between task executions (counted from start time of previous task execution)
     * @param task Function to be executed
     * @param args Task arguments
     */
    template <class T, class... Args>
    void fixed_rate_task(const std::string& name, Duration initial_delay, Duration period,
                       T&& task, Args&&... args) const {
        periodic_task<FixedRateSchedulePolicy<Clock>>(name, initial_delay, period,
                std::forward<T&&>(task), std::forward<Args&&>(args)...);
    }

    /*!
     * @brief Add self sheduling task.
     * @param name task name
     * @param initial_delay Initial delay of first execution of task
     * @param task Function to be executed, returns next run delay
     * @param args Task arguments
     */
    template <class T, class... Args>
    void self_scheduling_task(const std::string& name, Duration initial_delay,
                             T&& task, Args&&... args) const {
        std::function<decltype(task(std::forward<Args>(args)...))()> f = std::bind(std::forward<T>(task), std::forward<Args>(args)...);
        const auto task_name = create_task_name(name);
        log_debug("worker", "Schedule self sheduling task " << task_name << " delay: " << initial_delay.count());

        /* add task called on task own sheduling basis */
        m_tasks.emplace(task_name, [task_name, f] {
            try {
                return f();
            }
            catch (const StopProcessing&) {
                throw;
            }
            catch (const std::exception& e) {
                log_warning("worker", task_name << " raised exception: " << e.what());
            }
            catch (...) {
                log_warning("worker", task_name << " raised unknown exception");
            }
            return Duration::zero();
        }, initial_delay, FixedDelaySchedulePolicy<Clock>::next_timepoint);
    }

    /*!
     * @brief Add task run once.
     * @param task Task to be executed
     * @param args Task arguments
     * @warning Task is run only once (at current time)
     */
    template <class T, class... Args>
    void high_priority_task(const std::string& name, T task, Args... args) const {
        std::function <decltype(task(args...))()> f = std::bind(task, args...);
        const auto task_name = create_task_name(name);
        log_debug("worker", "Scheduled high priority task " << task_name);

        /* add single run task */
        m_tasks.emplace(task_name, [task_name, f] {
                try {
                    f();
                }
                catch (const std::exception& e) {
                    log_warning("worker", task_name << " raised exception: " << e.what());
                }
                catch (...) {
                    log_warning("worker", task_name << " raised unknown exception");
                }
                return Duration::zero();
            },
            /* schedule task as first one, it will be executed imediatelly */
            Timepoint::min());
    }


    /*!
     * @brief Add task run once.
     * @param task Task to be executed
     * @param args Task arguments
     * @warning Task is run only once (at current time)
     */
    template <class T, class... Args>
    void single_run_task(const std::string& name, Duration initial_delay, T task, Args... args) const {
        std::function <decltype(task(args...))()> f = std::bind(task, args...);
        const auto task_name = create_task_name(name);
        log_debug("worker", "Scheduled single run task " << task_name);

        /* add single run task */
        m_tasks.emplace(task_name, [task_name, f] {
            try {
                f();
            }
            catch (const std::exception& e) {
                log_warning("worker", task_name << " raised exception: " << e.what());
            }
            catch (...) {
                log_warning("worker", task_name << " raised unknown exception");
            }
            return Duration::zero();
        }, Clock::now() + initial_delay);
    }


private:

    template <class SchedulePolicy, class T, class... Args>
    void periodic_task(const std::string& name, Duration initial_delay, Duration period,
                       T&& task, Args&&... args) const {
        std::function<decltype(task(std::forward<Args>(args)...))()> f =
                            std::bind(std::forward<T>(task), std::forward<Args>(args)...);
        const auto task_name = create_task_name(name);
        log_debug("worker", "Schedule periodic task " << task_name
                << " delay: " << initial_delay.count()
                << " period: " << period.count()
                << " policy: " << static_cast<int>(SchedulePolicy::get_schedule_policy_type()));

        /* add task called periodically */
        m_tasks.emplace(task_name, [task_name, f, period] {
                try {
                    f();
                }
                catch (const StopProcessing&) {
                    throw;
                }
                catch (const std::exception& e) {
                    log_warning("worker", task_name << " raised exception: " << e.what());
                }
                catch (...) {
                    log_warning("worker", task_name << " raised unknown exception");
                }
                return period;
            }, initial_delay, SchedulePolicy::next_timepoint);
    }

    std::string create_task_name(const std::string& name = "") const {
        return name + ":" + m_name + ":" + std::to_string(m_counter++);
    }

    class Task final {
    public:
        using ExecuteFn = std::function<Duration()>;
        using CalculateTimepointFn = std::function<Timepoint(Duration, Timepoint)>;

        Task(const std::string& name, ExecuteFn&& t, Duration delay, CalculateTimepointFn policy)
            : m_name{name},
              m_execute{std::forward<ExecuteFn>(t)},
              m_timepoint{Clock::now() + delay},
              m_periodic{true},
              m_calculate_timepoint{policy} {}

        Task(const std::string& name, ExecuteFn&& t, Timepoint timepoint)
            : m_name{name},
              m_execute{std::forward<ExecuteFn>(t)},
              m_timepoint{timepoint},
              m_periodic{false},
              m_calculate_timepoint{FixedDelaySchedulePolicy<Clock>::next_timepoint} {}

        Task(Task&&) = default;
        Task& operator=(Task&&) = default;
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        const std::string& get_name() const {
            return m_name;
        }

        bool is_periodic() const {
            return m_periodic;
        }

        Duration operator()() const {
            return m_execute();
        }

        void set_delay(Duration delay) {
            m_timepoint = m_calculate_timepoint(delay, m_timepoint);
        }

        Duration get_delay() const {
            if (m_timepoint == Timepoint::min()) {
                return Duration::zero();
            }
            return m_timepoint - Clock::now();
        }

        bool operator>(const Task& o) const {
            return m_timepoint > o.m_timepoint;
        }

    private:
        std::string m_name;
        ExecuteFn m_execute;
        Timepoint m_timepoint;
        bool m_periodic;
        CalculateTimepointFn m_calculate_timepoint;
    };

    std::string m_name{};
    using Queue = DelayQueue<Task>;
    mutable Queue m_tasks{};
    std::atomic<bool> m_stop{false};
    mutable std::atomic<unsigned long> m_counter{};
    std::thread m_worker;
};

}
