/*!
 * @brief WorkerThread tests.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file test_worker_thread.cpp
 */

#include <mutex>
#include <condition_variable>
#include "generic/worker_thread.hpp"
#include "gtest/gtest.h"

template<int N>
class CountDownLatch {
public:
    void count_down() {
        std::unique_lock<std::mutex> lk(mutex);
        if (--counter <= 0) {
            cv.notify_all();
        }
    }

    int get_count() const {
        std::unique_lock<std::mutex> lk(mutex);
        return counter;
    }

    void wait() const {
        std::unique_lock<std::mutex> lk(mutex);
        cv.wait(lk, [this](){return counter <= 0;});
    }

    template<typename R, typename P>
    bool wait_for(const std::chrono::duration<R, P>& timeout) const {
        std::unique_lock<std::mutex> lk(mutex);
        return cv.wait_for(lk, timeout, [this](){return counter <= 0;});
    }

private:
    mutable std::mutex mutex{};
    mutable std::condition_variable cv{};
    int counter{N};
};

TEST(WorkerThreadTest, TasksAreExecuted) {
    constexpr const int NUM_TASKS = 5;
    CountDownLatch<NUM_TASKS> latch;
    EXPECT_EQ(NUM_TASKS, latch.get_count());

    auto task = [&latch]() {
        latch.count_down();
    };

    generic::WorkerThread worker{};
    for (int j = 0; j < NUM_TASKS; ++j) {
        worker(task);
    }
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));

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
    generic::WorkerThread::Duration delay = std::chrono::milliseconds(NUM_TASKS * MILLIS);
    for (int i = 0; i < NUM_TASKS; ++i) {
        worker(delay - std::chrono::milliseconds(i * MILLIS), task, i);
    }
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}


TEST(WorkerThreadTest, HighpriorityTask) {
    CountDownLatch<2> latch;
    auto task = [&latch] {
        latch.count_down();
    };
    generic::WorkerThread worker{};
    generic::WorkerThread::Duration delay = std::chrono::milliseconds(100);
    worker(delay, [&latch]{
        EXPECT_EQ(1, latch.get_count());
        latch.count_down();
    });
    worker.high_priority_task("high_priority", task);
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}


TEST(WorkerThreadTest, PeriodicTask) {
    CountDownLatch<3> latch;
    generic::WorkerThread worker{};
    auto task = [&latch, &worker] {
        latch.count_down();
    };
    generic::WorkerThread::Duration period = std::chrono::milliseconds(50);
    worker.periodic_task("periodic", generic::WorkerThread::Duration::zero(), period, task);
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
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

    generic::WorkerThread::Duration period = std::chrono::milliseconds(50);
    worker.periodic_task("periodic", generic::WorkerThread::Duration::zero(), period, task);
    /* latch send notification only when count down */
    EXPECT_FALSE(latch.wait_for(std::chrono::milliseconds(200)));
    EXPECT_EQ(1, latch.get_count());

    /* run the task again.. notification is to be sent here! */
    worker.periodic_task("continuation", generic::WorkerThread::Duration::zero(), period, task);
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());

    /* Test should just pass without deadlock regardless "clean_up" task is run. */
}

TEST(WorkerThreadTest, TasksWhichThrowExeptions) {
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
            throw std::runtime_error("failed");
        }
    };
    generic::WorkerThread worker{};
    worker(task, ExType::UNKNOWN);
    worker(task, ExType::RUNTIME);
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}
