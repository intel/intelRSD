/*
 * Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ReplicaRole;
import com.intel.podm.common.types.ReplicaType;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"replicaType", "replicaRole", "replica"})
@JsonInclude(NON_NULL)
public class ReplicaInfoDto {
    private ReplicaType replicaType;
    private ReplicaRole replicaRole;
    private Context replica;

    public ReplicaType getReplicaType() {
        return replicaType;
    }

    public void setReplicaType(ReplicaType replicaType) {
        this.replicaType = replicaType;
    }

    public ReplicaRole getReplicaRole() {
        return replicaRole;
    }

    public void setReplicaRole(ReplicaRole replicaRole) {
        this.replicaRole = replicaRole;
    }

    public Context getReplica() {
        return replica;
    }

    public void setReplica(Context replica) {
        this.replica = replica;
    }
}
