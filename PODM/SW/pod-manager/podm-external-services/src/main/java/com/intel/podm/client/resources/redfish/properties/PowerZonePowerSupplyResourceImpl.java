/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.resources.redfish.properties;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.api.resources.redfish.PowerZonePowerSupplyResource;
import com.intel.podm.client.api.resources.redfish.RackLocationObject;
import com.intel.podm.common.types.Status;

public class PowerZonePowerSupplyResourceImpl implements PowerZonePowerSupplyResource {
    @JsonProperty("Name")
    private String name;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("RackLocation")
    private RackLocationObjectImpl rackLocation;

    @JsonProperty("PowerCapacityWatts")
    private Integer powerCapacityWatts;

    @JsonProperty("LastPowerOutputWatts")
    private Integer lastPowerOutputWatts;

    @JsonProperty("Manufacturer")
    private String manufacturer;

    @JsonProperty("Model")
    private String modelNumber;

    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;

    @JsonProperty("SerialNumber")
    private String serialNumber;

    @JsonProperty("PartNumber")
    private String partNumber;

    @Override
    public String getName() {
        return name;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public RackLocationObject getRackLocation() {
        return rackLocation;
    }

    @Override
    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    @Override
    public Integer getLastPowerOutputWatts() {
        return lastPowerOutputWatts;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModelNumber() {
        return modelNumber;
    }

    @Override
    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public String getPartNumber() {
        return partNumber;
    }
}
