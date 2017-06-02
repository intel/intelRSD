/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */


#include "gtest/gtest.h"

#include "status/module_software_status.hpp"

namespace agent_framework {
namespace generic {


TEST(PingLocalhost_PingShouldBePositive, PingTest) {

    ModuleSoftwareStatus module_status;
    bool isPong = false;
    module_status.set_ip("127.0.0.1");
    isPong = module_status.ping();
    ASSERT_TRUE(isPong);
}

TEST(PingUndefinedAddress_PingShouldBeNegative, PingTest) {
    ModuleSoftwareStatus module_status;
    bool isPong = true;
    module_status.set_ip("8.8.8.8");
    isPong = module_status.ping();
    ASSERT_FALSE(isPong);
}
}
}
