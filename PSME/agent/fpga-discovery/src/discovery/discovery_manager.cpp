/*!
 * @brief Implementation of discovery manager.
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
 * @file discovery_manager.cpp
 */

// local includes
#include "discovery/discovery_manager.hpp"
#include "discovery/discovery.hpp"
#include "loader/config.hpp"
#include "tree_stability/fpga_stabilizer.hpp"

// includes from agent framework
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/version.hpp"

// system includes
#include <memory>

using namespace agent::fpga_discovery;
using namespace agent::fpga_discovery::loader;
using namespace agent::fpga_discovery::discovery;

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::eventing;
using namespace agent_framework::generic;

using namespace std;

namespace {
    static const string GUID_PATH = "/sys/class/dmi/id/product_uuid";

Uuid create_manager() {
    FpgaStabilizer ns{};
    Manager manager{};
    manager.set_status({enums::State::Enabled, enums::Health::OK});
    manager.set_firmware_version(Version::VERSION_STRING);
    manager.set_ipv4_address(FpgaDiscoveryConfig::get_instance()->get_ipv4_address());
    manager.add_collection({CollectionName::Fabrics, CollectionType::Fabrics});
    manager.add_collection({CollectionName::Systems, CollectionType::Systems});
    manager.add_collection({CollectionName::Chassis, CollectionType::Chassis});
    ns.stabilize(manager);
    get_manager<Manager>().add_entry(manager);
    return manager.get_uuid();
}

Uuid create_fabric(const Uuid& uuid) {
    FpgaStabilizer ns{};
    Fabric fabric{uuid};
    fabric.set_status({enums::State::Enabled, enums::Health::OK});
    fabric.set_protocol(enums::TransportProtocol::OEM);
    fabric.set_oem_protocol(enums::OemProtocol::FPGAoF);
    fabric.add_collection({CollectionName::Endpoints, CollectionType::Endpoints});
    fabric.add_collection({CollectionName::Zones, CollectionType::Zones});
    ns.stabilize(fabric);
    get_manager<Fabric>().add_entry(fabric);
    return fabric.get_uuid();
}

} // namespace

DiscoveryManager::DiscoveryManager() {}
DiscoveryManager::~DiscoveryManager() {}

Uuid DiscoveryManager::discover() {
    /* start discovery */
    auto manager_uuid = ::create_manager();
    auto fabric_uuid = ::create_fabric(manager_uuid);

    Discovery dw{};
    log_info("fpga-discovery-agent", "Running discovery");


    dw.discover(manager_uuid, fabric_uuid);

    send_add_notifications_for_each<Manager>();
    return manager_uuid;
}
