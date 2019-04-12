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
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.types.MemoryDeviceType;
import com.intel.rsd.nodecomposer.types.MemoryType;
import com.intel.rsd.nodecomposer.types.deserialization.PositiveIntegerDeserializer;

import java.io.Serializable;
import java.math.BigDecimal;

public final class RequestedMemoryImpl implements RequestedNode.Memory, Serializable {
    private static final long serialVersionUID = -827970284101510990L;

    @JsonProperty("CapacityMiB")
    private BigDecimal capacityMib;

    @JsonProperty
    private MemoryType memoryType;

    @JsonProperty
    private MemoryDeviceType memoryDeviceType;

    @JsonProperty("SpeedMHz")
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer speedMhz;

    @JsonProperty
    private String manufacturer;

    @JsonProperty
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer dataWidthBits;

    @JsonProperty("Resource")
    private ODataId resourceODataId;

    @JsonProperty("Chassis")
    private ODataId chassisODataId;

    @Override
    public MemoryType getMemoryType() {
        return memoryType;
    }

    @Override
    public BigDecimal getCapacityMib() {
        return capacityMib;
    }

    @Override
    public MemoryDeviceType getMemoryDeviceType() {
        return memoryDeviceType;
    }

    @Override
    public Integer getSpeedMhz() {
        return speedMhz;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public Integer getDataWidthBits() {
        return dataWidthBits;
    }

    @Override
    public ODataId getResourceODataId() {
        return resourceODataId;
    }

    @Override
    public ODataId getChassisODataId() {
        return chassisODataId;
    }
}
