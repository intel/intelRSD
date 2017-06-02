/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.redfish.PhysicalDriveResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

@OdataTypes({
    "#PhysicalDrive" + OdataTypes.VERSION_PATTERN + "PhysicalDrive"
})
public class PhysicalDriveResourceImpl extends ExternalServiceResourceImpl implements PhysicalDriveResource {
    @JsonProperty ("Interface")
    private StorageControllerInterface controllerInterface;
    @JsonProperty ("CapacityGiB")
    private Float capacityGib;
    @JsonProperty ("Type")
    private MediaType type;
    @JsonProperty("RPM")
    private Integer rpm;
    @JsonProperty ("Manufacturer")
    private String manufacturer;
    @JsonProperty ("Model")
    private String model;
    @JsonProperty ("SerialNumber")
    private String serialNumber;
    @JsonProperty ("Status")
    private Status status;

    @Override
    public StorageControllerInterface getControllerInterface() {
        return controllerInterface;
    }

    @Override
    public Float getCapacityGib() {
        return capacityGib;
    }

    @Override
    public MediaType getType() {
        return type;
    }

    @Override
    public Integer getRpm() {
        return rpm;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public Status getStatus() {
        return status;
    }
}
