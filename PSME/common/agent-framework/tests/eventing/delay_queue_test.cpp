/*!
 * @brief DelayQueue tests.
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
 * @file delay_queue_test.cpp
 */

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "generic/delay_queue.hpp"
#include "gtest/gtest.h"
#include "logger/logger_factory.hpp"
#include "count_down_latch.hpp"

using namespace generic;
using namespace std::chrono;

struct Elem {
    using DQ = DelayQueue<Elem>;
    using Timepoint = DQ::Timepoint;
    using Duration = DQ::Duration;

    Elem() = default;
    Elem(Duration d, const std::string& s)
        : m_timepoint(Timepoint::clock::now() + d), m_s(s) {}

    Duration get_delay() const {
        return m_timepoint - Timepoint::clock::now();
    }

    bool operator>(const Elem& e) const {
        return m_timepoint > e.m_timepoint;
    }

    Timepoint m_timepoint{};
    std::string m_s{};
};

class DelayQueueTest : public ::testing::Test {
protected:
    DelayQueue<Elem> m_queue{};

    virtual ~DelayQueueTest();
};

DelayQueueTest::~DelayQueueTest() { }

TEST_F(DelayQueueTest, EnqueueLastTo1st) {
    constexpr const int NUM_TASKS = 4;
    CountDownLatch<NUM_TASKS> latch;
    EXPECT_EQ(NUM_TASKS, latch.get_count());

    auto consume = [this, &latch] () {
        latch.count_down();
        auto elem = m_queue.wait_and_pop();
        ASSERT_EQ("1st", elem.m_s);
        latch.count_down();
        elem = m_queue.wait_and_pop();
        ASSERT_EQ("2nd", elem.m_s);
        latch.count_down();
        elem = m_queue.wait_and_pop();
        ASSERT_EQ("last", elem.m_s);
        latch.count_down();
    };

    std::thread consumer(consume);
    while (latch.get_count() == NUM_TASKS) {
        std::this_thread::sleep_for(milliseconds(1));
    }
    std::this_thread::sleep_for(milliseconds(100));

    auto last = Elem{milliseconds(500), "last"};
    auto e2 = Elem{milliseconds(300), "2nd"};
    auto e1 = Elem{milliseconds(200), "1st"};
    m_queue.push(last);
    std::this_thread::sleep_for(milliseconds(100));
    m_queue.push(e2);
    std::this_thread::sleep_for(milliseconds(100));
    m_queue.push(e1);

    consumer.join();
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}

TEST_F(DelayQueueTest, Enqueue1stToLast) {
    constexpr const int NUM_TASKS = 4;
    CountDownLatch<NUM_TASKS> latch;
    EXPECT_EQ(NUM_TASKS, latch.get_count());

    auto consume = [this, &latch] () {
        latch.count_down();
        auto elem = m_queue.wait_and_pop();
        ASSERT_EQ("1st", elem.m_s);
        latch.count_down();
        elem = m_queue.wait_and_pop();
        ASSERT_EQ("2nd", elem.m_s);
        latch.count_down();
        elem = m_queue.wait_and_pop();
        ASSERT_EQ("last", elem.m_s);
        latch.count_down();
    };

    std::thread consumer(consume);
    while (latch.get_count() == NUM_TASKS) {
        std::this_thread::sleep_for(milliseconds(1));
    }
    std::this_thread::sleep_for(milliseconds(100));

    auto last = Elem{milliseconds(500), "last"};
    auto e2 = Elem{milliseconds(300), "2nd"};
    auto e1 = Elem{milliseconds(200), "1st"};
    m_queue.push(e1);
    std::this_thread::sleep_for(milliseconds(100));
    m_queue.push(e2);
    std::this_thread::sleep_for(milliseconds(100));
    m_queue.push(last);

    consumer.join();
    EXPECT_TRUE(latch.wait_for(std::chrono::seconds(10)));
    EXPECT_EQ(0, latch.get_count());
}
