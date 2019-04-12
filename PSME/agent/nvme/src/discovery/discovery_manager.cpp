/*!
 * @brief Implementation of discovery manager.
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
 * @file discovery_manager.cpp
 */

// local includes
#include "discovery/discovery_manager.hpp"
#include "discovery/discovery_initiator.hpp"
#include "discovery/discovery_target.hpp"
#include "tools/endpoint_reader.hpp"
#include "tools/endpoint_creator.hpp"
#include "tools/unix_dir.hpp"
#include "loader/config.hpp"
#include "tree_stability/nvme_stabilizer.hpp"

// includes from agent framework
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/version.hpp"

// system includes
#include <memory>



using namespace agent::nvme;
using namespace agent::nvme::loader;
using namespace agent::nvme::discovery;
using namespace agent::nvme::tools;

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
    NvmeStabilizer ns{};
    Manager manager{};
    manager.set_status({enums::State::Enabled, enums::Health::OK});
    manager.set_firmware_version(Version::VERSION_STRING);
    manager.set_ipv4_address(NvmeConfig::get_instance()->get_ipv4_address());
    manager.add_collection({CollectionName::Fabrics, CollectionType::Fabrics});
    manager.add_collection({CollectionName::Systems, CollectionType::Systems});
    manager.add_collection({CollectionName::Chassis, CollectionType::Chassis});
    ns.stabilize(manager);
    get_manager<Manager>().add_entry(manager);
    return manager.get_uuid();
}


Uuid create_fabric(const Uuid& uuid) {
    NvmeStabilizer ns{};
    Fabric fabric{uuid};
    fabric.set_status({enums::State::Enabled, enums::Health::OK});
    fabric.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    fabric.add_collection({CollectionName::Endpoints, CollectionType::Endpoints});
    fabric.add_collection({CollectionName::Zones, CollectionType::Zones});
    ns.stabilize(fabric);
    get_manager<Fabric>().add_entry(fabric);
    return fabric.get_uuid();
}


Uuid create_chassis(const Uuid& manager_uuid) {
    /* add chassis */
    NvmeStabilizer ns{};
    Chassis chassis_model{manager_uuid};
    chassis_model.set_type(enums::ChassisType::Enclosure);
    chassis_model.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    chassis_model.set_location_offset(NvmeConfig::get_instance()->get_location_offset());
    chassis_model.set_parent_id(NvmeConfig::get_instance()->get_parent_id());
    chassis_model.add_collection({CollectionName::Drives, CollectionType::Drives});

    log_debug("nvme-agent", "Created Chassis module");
    ns.stabilize(chassis_model);
    get_manager<Chassis>().add_entry(chassis_model);
    return chassis_model.get_uuid();
}


Uuid create_system(const Uuid& manager_uuid, const Uuid& chassis_uuid) {
    NvmeStabilizer ns{};
    UnixDir dir{};
    System system{manager_uuid};
    system.set_system_type(enums::SystemType::Virtual);
    system.set_chassis(chassis_uuid);
    system.add_collection({CollectionName::NetworkInterfaces, CollectionType::NetworkInterfaces});
    system.add_collection({CollectionName::StorageServices, CollectionType::StorageServices});

    try {
        system.set_guid(dir.read_first_line(GUID_PATH));
    }
    catch (const exception& e) {
        log_error("nvme-agent", string("Unable to get system GUID: ") + e.what());
    }
    ns.stabilize(system);
    get_manager<System>().add_entry(system);
    return system.get_uuid();
}

} // namespace

DiscoveryManager::DiscoveryManager() {}


DiscoveryManager::~DiscoveryManager() {}


Uuid DiscoveryManager::discover(std::shared_ptr<NvmeAgentContext> context) {
    /* start discovery */
    auto manager_uuid = ::create_manager();
    Uuid chassis_uuid = ::create_chassis(manager_uuid);
    Uuid system_uuid = ::create_system(manager_uuid, chassis_uuid);
    auto fabric_uuid = ::create_fabric(manager_uuid);
    EndpointReader er{};
    EndpointCreator ec{};
    DiscoveryTarget dt{context, er, ec};

    log_info("nvme-agent", "Running target discovery");
    dt.discover(manager_uuid, fabric_uuid);

    send_add_notifications_for_each<Manager>();
    return manager_uuid;
}
