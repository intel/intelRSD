/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */

#include "agent-framework/generic/obj_reference.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <future>

using namespace agent_framework;
using namespace agent_framework::generic;

namespace {

class Resource {
public:
    void touch() {
        thread_touched = true;
    }
    bool thread_touched = { false };
};


template <typename T>
class Manager {
public:
    ObjReference<T, std::recursive_mutex> get_obj_ref() {
        return ObjReference<T, std::recursive_mutex>(res, m_mutex);
    }
    Resource res {};
    mutable std::recursive_mutex m_mutex{};
};


class ObjReferenceTest : public ::testing::Test {
public:
    void touch() {
        auto objr = mgr.get_obj_ref();
        objr->touch();
    }
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    virtual ~ObjReferenceTest();


    Manager<Resource> mgr {};
};

ObjReferenceTest::~ObjReferenceTest() { }


}

TEST_F(ObjReferenceTest, OtherThreadIsBlockedUntilObjReferenceIsReleased) {
    std::unique_ptr<std::thread> t = nullptr;
    {
        // hold mutex
        auto objr = mgr.get_obj_ref();

        // start thread that should be blocked until obj ref is released
        t.reset(new std::thread(&ObjReferenceTest::touch, this));

        // thread might be started with some delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_FALSE(mgr.res.thread_touched); // thread is expected to be blocked
    } // RAII - mutex is released

    if (t->joinable()) {
        t->join();
    }

    ASSERT_TRUE(mgr.res.thread_touched);
}

TEST_F(ObjReferenceTest, OtherThreadIsBlockedUntilAllObjReferencesAreReleased) {
    ObjReference<Resource, std::recursive_mutex>* objr_copy = nullptr;
    std::unique_ptr<std::thread> t = nullptr;
    {
        ObjReference<Resource, std::recursive_mutex> objr = mgr.get_obj_ref();
        objr_copy = new ObjReference<Resource, std::recursive_mutex>(objr);

        // start thread that should block until all obj refs are released
        t.reset(new std::thread(&ObjReferenceTest::touch, this));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ASSERT_FALSE(mgr.res.thread_touched);
    } // RAII - objr is released, but objr_copy still exists and holds mutex

    // still should be blocked (objr_copy still exists)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(mgr.res.thread_touched);

    delete objr_copy; // another thread should be able to access the Resource after this delete

    // all object references are release. now we can wait
    if (t->joinable()) {
        t->join();
    }
    ASSERT_TRUE(mgr.res.thread_touched);
}

