/*!
 * @brief Implementation of FpgaContext class.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file fpga_context.cpp
 */

#include "tools/fpga_discovery_context.hpp"
#include "tools/fpga_discovery_database.hpp"

using namespace agent::fpga_discovery::tools;
using namespace std;

FpgaDiscoveryContext::~FpgaDiscoveryContext() {}

Context::DatabasePtr FpgaDiscoveryContext::create_database(const Uuid& resource, const string& name) const {
    DatabasePtr db{new FpgaDiscoveryDatabase(resource, name)};
    return db;
}

