/*!
 * @brief Implementation of Nvme config stub tests.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file nvme_config_stub.cpp
 */

#include "loader/config.hpp"

using namespace agent::nvme::loader;
using namespace std;

NvmeConfig::~NvmeConfig() { }

static string parent_id{"1"};

void NvmeConfig::set_parent_id(const string& str) {
    parent_id = str;
}

string NvmeConfig::get_parent_id() {
    return parent_id;
}

void NvmeConfig::set_endpoint_nvme_port(const Uuid&, uint16_t) {}

