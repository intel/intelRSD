/*!
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
 * */
#include "agent-framework/exceptions/exception.hpp"
#include "lvm/lvm_create_data.hpp"

class LvmAPI::LvmImpl {
public:
    LvmAPI::VolumeGroupVec discover_volume_groups() {
        return {};
    }

    void create_snapshot(const LvmCreateData&) {}

    void create_clone(const LvmCreateData&) {}

    void remove_logical_volume(const std::string&, const std::string&) {}
};

