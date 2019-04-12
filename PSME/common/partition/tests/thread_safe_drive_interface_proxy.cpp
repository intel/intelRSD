/*!
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
 * @file tests/thread_safe_drive_interface_proxy.cpp
 */

#include "partition/thread_safe_drive_interface_proxy.hpp"

#include "gtest/gtest.h"

#include <thread>
#include <list>
#include <map>

using namespace partition;
using namespace utils;

/*
 * These tests are meant to be run by valgrind and it should not report any issues.
 * valgrind --tool=drd ./TEST_BINARY --gtest_filter=ThreadSafeDrive*
 */

namespace {

class SingleThreadMockDriveInterface final : public AbstractDriveInterface {
public:

    SingleThreadMockDriveInterface() {
        m_writes["0"] = 0;
        m_writes["1"] = 0;
        m_writes["2"] = 0;
        m_writes["3"] = 0;
    }

    SingleThreadMockDriveInterface(const SingleThreadMockDriveInterface&) = default;

    SingleThreadMockDriveInterface& operator=(const SingleThreadMockDriveInterface&) = default;

    virtual ~SingleThreadMockDriveInterface() {}

    /*! Implementations of pure virtual functions */
    virtual ByteBuffer read(const std::string&, uint64_t, std::size_t) const override {
        throw std::runtime_error("No implementation for tests");
    }

    virtual void write(const std::string& target, uint64_t, const ByteBuffer&) const override {
        // this is very not thread-safe, proxy will make it safe
        unsigned tmp = m_writes[target];
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_writes[target] = tmp + 1;
    }

    virtual void write(const std::string&, uint64_t, uint64_t, DataGenerator) const override {
        throw std::runtime_error("No implementation for tests");
    }

    int get(const std::string& id) const {
        return m_writes[id];
    }

private:

    mutable std::map<std::string, int> m_writes{};

};

}

namespace testing {

TEST(ThreadSafeDriveInterfaceProxy, SingleFileMultipleThreads) {
    std::shared_ptr<SingleThreadMockDriveInterface> mdi = std::make_shared<SingleThreadMockDriveInterface>();
    ThreadSafeDriveInterfaceProxy tsp(mdi);

    static constexpr unsigned THREAD_NUM = 40;
    std::list<std::thread> thread_list{};
    for (unsigned i = 0; i < THREAD_NUM; ++i) {
        thread_list.emplace_back([&]{tsp.write("1", 0, ByteBuffer{});});
    }
    for (auto& thread : thread_list) {
        thread.join();
    }
    EXPECT_EQ(mdi->get("0"), 0);
    EXPECT_EQ(mdi->get("1"), THREAD_NUM);
    EXPECT_EQ(mdi->get("2"), 0);
    EXPECT_EQ(mdi->get("3"), 0);
}

TEST(ThreadSafeDriveInterfaceProxy, MultipleFilesMultipleThreads) {
    std::shared_ptr<SingleThreadMockDriveInterface> mdi = std::make_shared<SingleThreadMockDriveInterface>();
    ThreadSafeDriveInterfaceProxy tsp(mdi);

    static constexpr unsigned THREAD_NUM = 40;
    std::list<std::thread> thread_list{};
    for (unsigned i = 0; i < THREAD_NUM / 4; ++i) {
        thread_list.emplace_back([&]{tsp.write("0", 0, ByteBuffer{});});
        thread_list.emplace_back([&]{tsp.write("1", 0, ByteBuffer{});});
        thread_list.emplace_back([&]{tsp.write("2", 0, ByteBuffer{});});
        thread_list.emplace_back([&]{tsp.write("3", 0, ByteBuffer{});});
    }
    for (auto& thread : thread_list) {
        thread.join();
    }
    EXPECT_EQ(mdi->get("0"), THREAD_NUM / 4);
    EXPECT_EQ(mdi->get("1"), THREAD_NUM / 4);
    EXPECT_EQ(mdi->get("2"), THREAD_NUM / 4);
    EXPECT_EQ(mdi->get("3"), THREAD_NUM / 4);
}

}
