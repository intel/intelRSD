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

#include "agent-framework/module/model/acl.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Acl::component = enums::Component::Acl;
const enums::CollectionName Acl::collection_name = enums::CollectionName::Acls;

Acl::Acl(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Acl::~Acl() {}

json::Json Acl::to_json() const {
    json::Json result = json::Json();
    result[literals::Acl::STATUS] = get_status().to_json();
    result[literals::Acl::COLLECTIONS] = get_collections().to_json();
    result[literals::Acl::OEM] = get_oem().to_json();
    return result;
}

Acl Acl::from_json(const json::Json& json) {
    Acl acl{};
    acl.set_status(attribute::Status::from_json(json[literals::Acl::STATUS]));
    acl.set_collections(Collections::from_json(json[literals::Acl::COLLECTIONS]));
    acl.set_oem(attribute::Oem::from_json(json[literals::Acl::OEM]));
    return acl;
}
