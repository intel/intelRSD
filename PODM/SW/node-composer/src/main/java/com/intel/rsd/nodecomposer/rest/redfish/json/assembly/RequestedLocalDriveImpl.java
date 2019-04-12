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
import com.intel.rsd.nodecomposer.types.MediaType;
import com.intel.rsd.nodecomposer.types.Protocol;

import java.io.Serializable;
import java.math.BigDecimal;

public final class RequestedLocalDriveImpl implements RequestedNode.LocalDrive, Serializable {
    private static final long serialVersionUID = 8972850107706173858L;

    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;

    @JsonProperty
    private MediaType type;

    @JsonProperty("MinRPM")
    private BigDecimal minRpm;

    @JsonProperty
    private String serialNumber;

    @JsonProperty("Interface")
    private Protocol protocol;

    @JsonProperty("Resource")
    private ODataId resourceODataId;

    @JsonProperty("Chassis")
    private ODataId chassisODataId;

    @JsonProperty("FabricSwitch")
    private Boolean isFromFabricSwitch;

    @Override
    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    @Override
    public MediaType getType() {
        return type;
    }

    @Override
    public BigDecimal getMinRpm() {
        return minRpm;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public Protocol getInterface() {
        return protocol;
    }

    @Override
    public ODataId getResourceODataId() {
        return resourceODataId;
    }

    @Override
    public ODataId getChassisODataId() {
        return chassisODataId;
    }

    @Override
    public Boolean isFromFabricSwitch() {
        return isFromFabricSwitch;
    }
}
