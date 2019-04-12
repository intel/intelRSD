/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#include "agent-framework/module/model/fabric.hpp"
#include "agent-framework/module/constants/pnc.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component Fabric::component = enums::Component::Fabric;
const enums::CollectionName Fabric::collection_name =
    enums::CollectionName::Fabrics;

Fabric::Fabric(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Fabric::~Fabric() {}

json::Json Fabric::to_json() const {
    json::Json result = json::Json();
    result[literals::Switch::COLLECTIONS] = get_collections().to_json();
    result[literals::Switch::STATUS] = get_status().to_json();
    result[literals::Switch::OEM] = get_oem().to_json();
    result[literals::Switch::PROTOCOL] = get_protocol();
    result[literals::Endpoint::OEM_PROTOCOL] = get_oem_protocol();
    return result;
}

Fabric Fabric::from_json(const json::Json& json) {
    Fabric pcie_fabric{};

    pcie_fabric.set_collections(Collections::from_json(json[literals::Switch::COLLECTIONS]));
    pcie_fabric.set_status(Status::from_json(json[literals::Switch::STATUS]));
    pcie_fabric.set_oem(Oem::from_json(json[literals::Switch::OEM]));
    pcie_fabric.set_protocol(json[literals::Switch::PROTOCOL]);
    pcie_fabric.set_oem_protocol(json[literals::Endpoint::OEM_PROTOCOL]);

    return pcie_fabric;
}
