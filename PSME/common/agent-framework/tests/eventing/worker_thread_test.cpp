/*!
 * @brief WorkerThread tests.
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
 * @file test_worker_thread.cpp
 */

#include <mutex>
#include <condition_variable>
#include "generic/worker_thread.hpp"
#include "gtest/gtest.h"
#include "count_down_latch.hpp"

using namespace std::chrono;

TEST(WorkerThreadTest, TasksAreExecuted) {
    constexpr const int NUM_TASKS = 5;
    CountDownLatch<NUM_TASKS> latch;
    EXPECT_EQ(NUM_TASKS, latch.get_count());
    auto task = [&latch](int expected) {
        EXPECT_EQ(expected, latch.get_count());
        latch.count_down();
    };

    generic::WorkerThread worker{};
    for (int j = 0; j < NUM_TASKS; ++j) {
        if (j % 2 == 0) {
            worker(generic::WorkerThread::Duration::zero(), task, NUM_TASKS - j);
        }
        else {
            worker(task, NUM_TASKS - j);
        }
    }
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, DelayedTask) {
    constexpr const int NUM_TASKS = 3;
    constexpr const int MILLIS = 100;
    CountDownLatch<NUM_TASKS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch] (int expected) {
        latch.count_down();
        EXPECT_EQ(expected, latch.get_count());
    };
    generic::WorkerThread::Duration delay = milliseconds(NUM_TASKS * MILLIS);
    for (int i = 0; i < NUM_TASKS; ++i) {
        worker(delay - milliseconds(i * MILLIS), task, i);
    }
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, HighpriorityTask) {
    CountDownLatch<2> latch;
    auto task = [&latch] {
        latch.count_down();
    };
    generic::WorkerThread worker{};
    generic::WorkerThread::Duration delay = milliseconds(100);
    worker(delay, [&latch]{
        EXPECT_EQ(1, latch.get_count());
        latch.count_down();
    });
    worker.high_priority_task("high_priority", task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, FixedRateTask) {
    constexpr int NUM_RUNS = 3;
    constexpr int TASK_EXEC_TIME = 25;
    constexpr int DELAY = 50;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &TASK_EXEC_TIME] {
        std::this_thread::sleep_for(milliseconds(TASK_EXEC_TIME));
        latch.count_down();
    };
    generic::WorkerThread::Duration period = milliseconds(DELAY);
    auto start = generic::WorkerThread::Clock::now();
    worker.fixed_rate_task("fixed_rate_task", generic::WorkerThread::Duration::zero(), period, task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
    auto elapsed = duration_cast<milliseconds>(generic::WorkerThread::Clock::now() - start);
    EXPECT_LE(milliseconds(TASK_EXEC_TIME + (NUM_RUNS-1)*DELAY).count(), elapsed.count());
}

TEST(WorkerThreadTest, FixedDelayTask) {
    constexpr int NUM_RUNS = 3;
    constexpr int TASK_EXEC_TIME = 25;
    constexpr int DELAY = 50;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &TASK_EXEC_TIME] {
        std::this_thread::sleep_for(milliseconds(TASK_EXEC_TIME));
        latch.count_down();
    };
    generic::WorkerThread::Duration period = milliseconds(DELAY);
    auto start = generic::WorkerThread::Clock::now();
    worker.fixed_delay_task("fixed_delay_task", generic::WorkerThread::Duration::zero(), period, task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
    auto elapsed = duration_cast<milliseconds>(generic::WorkerThread::Clock::now() - start);
    EXPECT_LE(milliseconds(NUM_RUNS*TASK_EXEC_TIME+(NUM_RUNS-1)*DELAY).count(), elapsed.count());
}

TEST(WorkerThreadTest, SelfSchedulingTask) {
    constexpr int NUM_RUNS = 3;
    constexpr int DELAY = 50;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &DELAY] {
        latch.count_down();
        generic::WorkerThread::Duration delay = milliseconds(DELAY);
        return delay;
    };
    worker.self_scheduling_task("self_scheduling_task", generic::WorkerThread::Duration::zero(), task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, SelfSchedulingTaskNegativeDuration) {
    constexpr int NUM_RUNS = 3;
    constexpr int DELAY = -50;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &DELAY] {
        latch.count_down();
        generic::WorkerThread::Duration delay = milliseconds(DELAY);
        return delay;
    };
    worker.self_scheduling_task("self_scheduling_task", generic::WorkerThread::Duration::zero(), task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, SelfSchedulingTaskThrowsRuntimeError) {
    constexpr int NUM_RUNS = 3;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch] () -> generic::WorkerThread::Duration {
        latch.count_down();
        throw std::runtime_error("runtime_error");
    };
    worker.self_scheduling_task("self_scheduling_task", generic::WorkerThread::Duration::zero(), task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, SelfSchedulingTaskThrowsStopProcessing) {
    constexpr int NUM_RUNS = 1;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch] () -> generic::WorkerThread::Duration {
        latch.count_down();
        throw generic::WorkerThread::StopProcessing();
    };
    worker.self_scheduling_task("self_scheduling_task", generic::WorkerThread::Duration::zero(), task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, SelfSchedulingTaskThrowsUnknownError) {
    constexpr int NUM_RUNS = 3;
    CountDownLatch<NUM_RUNS> latch;
    generic::WorkerThread worker{};
    auto task = [&latch] () -> generic::WorkerThread::Duration {
        latch.count_down();
        throw 1;
    };
    worker.self_scheduling_task("self_scheduling_task", generic::WorkerThread::Duration::zero(), task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, RaceConditionWithDestructor) {
    CountDownLatch<3> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &worker] {
        latch.count_down();
        if (latch.get_count() == 1) {
            throw generic::WorkerThread::StopProcessing();
        }
        if (latch.get_count() == 0) {
            worker.high_priority_task("clean_up", [&worker] {
                worker.remove_tasks();
            });
        }
    };

    generic::WorkerThread::Duration period = milliseconds(50);
    worker.fixed_delay_task("periodic", generic::WorkerThread::Duration::zero(), period, task);
    /* latch send notification only when count down */
    EXPECT_FALSE(latch.wait_for(milliseconds(200)));
    EXPECT_EQ(1, latch.get_count());

    /* run the task again.. notification is to be sent here! */
    worker.fixed_delay_task("continuation", generic::WorkerThread::Duration::zero(), period, task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());

    /* Test should just pass without deadlock regardless "clean_up" task is run. */
}

TEST(WorkerThreadTest, ExceptionThrownFromTask) {
    CountDownLatch<2> latch;
    enum class ExType {
        UNKNOWN,
        RUNTIME
    };
    auto task = [&latch] (ExType type) {
        latch.count_down();
        if (ExType::UNKNOWN == type) {
            throw 1;
        }
        else {
            throw std::runtime_error("runtime_error");
        }
    };
    generic::WorkerThread worker{};
    worker(task, ExType::UNKNOWN);
    worker(task, ExType::RUNTIME);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST(WorkerThreadTest, Stop) {
    generic::WorkerThread worker{};
    EXPECT_TRUE(worker.is_running());
    worker.stop();
    EXPECT_FALSE(worker.is_running());
}

TEST(WorkerThreadTest, StopCalledFromWorkerThread) {
    CountDownLatch<1> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &worker] () {
        latch.count_down();
        worker.stop();
    };
    worker(task);
    EXPECT_TRUE(latch.wait_for(seconds(10)));
    EXPECT_EQ(0, latch.get_count());
    EXPECT_TRUE(worker.is_running());
}
