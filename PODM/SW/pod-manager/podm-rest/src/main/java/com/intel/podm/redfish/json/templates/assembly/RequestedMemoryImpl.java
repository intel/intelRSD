/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.deserialization.PositiveIntegerDeserializer;

import java.math.BigDecimal;

import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;

public final class RequestedMemoryImpl implements RequestedNode.Memory {
    @JsonProperty("CapacityMiB")
    private BigDecimal capacityMib;

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

    private Context resourceContext;

    private Context chassisContext;

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

    @JsonProperty("Resource")
    public void setResourceContext(ODataId resource) {
        if (resource == null) {
            return;
        }

        resourceContext = getContextFromUri(resource.toUri(), MEMORY);
    }

    @Override
    public Context getResourceContext() {
        return resourceContext;
    }

    @JsonProperty("Chassis")
    public void setChassisContext(ODataId chassis) {
        if (chassis == null) {
            return;
        }

        chassisContext = getContextFromUri(chassis.toUri(), CHASSIS);
    }

    @Override
    public Context getChassisContext() {
        return chassisContext;
    }
}
