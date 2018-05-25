/*!
 * @brief Implementation of NvmeTransport class.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file nvme_transport.cpp
 */

#include "tools/nvme_transport.hpp"
#include "nvmfpp/file_fabric_interface.hpp"
#include "nvmfpp/file_factory.hpp"
#include "agent-framework/module/model/attributes/identifier.hpp"
#include "sysfs/sysfs_interface.hpp"
#include "tools/drive_discovery/drive_reader.hpp"
#include "tools/tools.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace nvmfpp;
using namespace std;

namespace {
FabricProperties get_fabric_properties(const Endpoint& target) {
    const auto& ip_transport_details = target.get_ip_transport_details();
    if (ip_transport_details.size() != 1) {
        throw logic_error("Exactly one transport is expected in endpoint");
    }

    const auto& ip_transport_detail = ip_transport_details.front();

    auto subnqn = attribute::Identifier::get_nqn(target);
    tools::convert_to_subnqn(subnqn);

    FabricProperties fabric_properties{subnqn,
                                       ip_transport_detail.get_ipv4_address().get_address(),
                                       uint16_t(ip_transport_detail.get_port()), Transport::RDMA};
    return fabric_properties;
}
}

void NvmeTransport::connect(const Endpoint& target) {
    unique_ptr<BaseFabricInterface> fabric = make_unique<FileFabricInterface>(make_unique<FileFactory>());
    auto fabric_properties = get_fabric_properties(target);
    fabric->connect(fabric_properties);
}

void NvmeTransport::disconnect(const Endpoint& target) {
    unique_ptr<BaseFabricInterface> fabric = make_unique<FileFabricInterface>(make_unique<FileFactory>());
    auto fabric_properties = get_fabric_properties(target);
    tools::DriveReader dr{make_shared<sysfs::SysfsInterface>()};
    fabric->disconnect_by_nqn(fabric_properties, dr.get_drives());
}
