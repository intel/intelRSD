/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.StorageControllerInterface;
import com.intel.podm.common.types.deserialization.PositiveIntegerDeserializer;

import java.math.BigDecimal;

import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.business.services.context.ContextType.SIMPLE_STORAGE;
import static com.intel.podm.redfish.UriConverter.getContextFromUri;

public final class RequestedLocalDriveImpl implements RequestedLocalDrive {
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;

    @JsonProperty
    private DriveType type;

    @JsonProperty("MinRPM")
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer minRpm;

    @JsonProperty
    private String serialNumber;

    @JsonProperty("Interface")
    private StorageControllerInterface storageControllerInterface;

    private Context resourceContext;

    private Context chassisContext;

    @Override
    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    @Override
    public DriveType getType() {
        return type;
    }

    @Override
    public Integer getMinRpm() {
        return minRpm;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public StorageControllerInterface getInterface() {
        return storageControllerInterface;
    }

    @JsonProperty("Resource")
    public void setResourceContext(ODataId resource) {
        if (resource == null) {
            return;
        }

        resourceContext = getContextFromUri(resource.toUri(), DEVICE, SIMPLE_STORAGE);
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
