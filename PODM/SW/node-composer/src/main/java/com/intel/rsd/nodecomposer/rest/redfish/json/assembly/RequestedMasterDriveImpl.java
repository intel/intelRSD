/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.json.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.types.ReplicaType;

import java.io.Serializable;

public final class RequestedMasterDriveImpl implements RequestedNode.RemoteDrive.MasterDrive, Serializable {
    private static final long serialVersionUID = 4413831292025217552L;

    @JsonProperty
    private ReplicaType type;

    @JsonProperty("Resource")
    private ODataId resourceODataId;

    @Override
    public ReplicaType getType() {
        return type;
    }

    @Override
    public ODataId getResourceODataId() {
        return resourceODataId;
    }
}
