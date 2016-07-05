/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#ifndef MOCK_MODULE_STATUS_HPP
#define MOCK_MODULE_STATUS_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "agent-framework/status/module_status.hpp"

namespace agent_framework {
namespace testing {

class MockModuleStatus : public agent_framework::status::ModuleStatus {
public:
    MOCK_METHOD0(read_status, Status());
    ~MockModuleStatus();
};

}
}

#endif /* MOCK_MODULE_STATUS_HPP */
