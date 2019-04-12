/*!
 * @brief Drive discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_drive_discoverer.cpp
 */

#include "discovery/builders/spdk_drive_builder.hpp"
#include "discovery/discoverers/spdk_drive_discoverer.hpp"
#include "spdk/spdk_api.hpp"
#include "spdk/model/bdev.hpp"



using namespace spdk;
using namespace agent::spdk;
using namespace agent::spdk::discovery;
using namespace agent_framework::model;

std::vector<Drive> discovery::SpdkDriveDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<Drive> drives{};
    std::vector<model::Bdev> bdevs{};

    try {
        bdevs = m_context->spdk_api->get_bdevs();
    }
    catch (const std::exception& exception) {
        log_error("spdk-discovery", "Failed to call get_bdevs method on SPDK: " << exception.what());
    }

    for (const auto& bdev : bdevs) {
        try {
            if (bdev.get_product_name() == model::BdevProductName::NVMe_DISK) {
                log_debug("spdk-discovery", "\n" << bdev.to_string());
                auto drive = SpdkDriveBuilder::build_default(parent_uuid);
                SpdkDriveBuilder::update_with_spdk(drive, bdev);
                drives.emplace_back(std::move(drive));
            }
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery", "Failed to convert drive model from SPDK BDEV: " << exception.what());
        }
    }

    return drives;
}
