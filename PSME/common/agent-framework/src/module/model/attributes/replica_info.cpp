/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/model/attributes/replica_info.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


ReplicaInfo::~ReplicaInfo() {}


json::Json ReplicaInfo::to_json() const {
    json::Json result = json::Json();
    result[literals::ReplicaInfo::REPLICA] = get_replica();
    result[literals::ReplicaInfo::REPLICA_READ_ONLY_ACCESS] = get_replica_read_only_access();
    result[literals::ReplicaInfo::REPLICA_ROLE] = get_replica_role();
    result[literals::ReplicaInfo::REPLICA_TYPE] = get_replica_type();
    return result;
}


ReplicaInfo ReplicaInfo::from_json(const json::Json& json) {
    attribute::ReplicaInfo replica_info{};
    replica_info.set_replica(json[literals::ReplicaInfo::REPLICA]);
    replica_info.set_replica_read_only_access(json[literals::ReplicaInfo::REPLICA_READ_ONLY_ACCESS]);
    replica_info.set_replica_role(json[literals::ReplicaInfo::REPLICA_ROLE]);
    replica_info.set_replica_type(json[literals::ReplicaInfo::REPLICA_TYPE]);
    return replica_info;
}
