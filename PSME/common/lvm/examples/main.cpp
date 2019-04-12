/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * */

#include "lvm/lvm_api.hpp"
#include "logger/logger.hpp"

int main() {
    storage::lvm::LvmApi api{};

    auto vgs = api.discover_volume_groups(storage::lvm::DiscoveryType::VG_LV);
    if (vgs.empty()) {
        log_notice("lvm-example", "No VGS found - aborting.");
        return -1;
    }

    auto vg = vgs.front();
    log_notice("lvm-example", "Discovered VolumeGroup: \n" + vg.to_string());

    storage::lvm::model::CreationData cd{};
    cd.set_create_name("example-volume");
    cd.set_size(4194304u);
    cd.set_volume_group(vg.get_name());

    auto lv = api.create_volume(cd);
    log_notice("lvm-example", "Volume created: " + lv.to_string());

    auto new_size = 8388608u;
    log_notice("lvm-example", "Trying to resize volume: \n" + std::to_string(new_size));
    auto lv_resized = api.resize_volume(vg.get_name(), lv.get_name(), new_size);
    log_notice("lvm-example", "Volume successfully resized: \n" + lv_resized.to_string());

    api.remove_logical_volume(vg.get_name(), lv.get_name());
    log_notice("lvm-example", "Volume successfully removed.");

    return 0;
}