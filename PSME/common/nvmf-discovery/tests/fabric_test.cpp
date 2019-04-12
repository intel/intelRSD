/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file fabric_test.cpp
 */

#include "nvmf-discovery/fabric.hpp"
#include "nvmf-discovery/interface.hpp"
#include "gtest/gtest.h"

using namespace nvmf_discovery;

namespace {

constexpr const char* SOCKETS_PROVIDER = "sockets";
constexpr const char* LOCALHOST = "127.0.0.1";

}

TEST(FabricTest, GetFiInfoSocketsProvider) {
    Interface iface{};
    iface.provider = SOCKETS_PROVIDER;
    iface.traddr = LOCALHOST;
    auto hints = prepare_fabric_hints(SOCKETS_PROVIDER);
    auto info = get_fi_info(iface, *hints, 0);
    EXPECT_TRUE(info.get() != nullptr);
}
