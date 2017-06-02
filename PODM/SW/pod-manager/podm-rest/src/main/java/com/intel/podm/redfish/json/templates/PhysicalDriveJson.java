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
package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "controllerInterface", "capacityGib", "type", "rpm", "manufacturer",
    "model", "serialNumber", "firmwareVersion", "status", "oem", "links"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public final class PhysicalDriveJson extends BaseResourceJson {
    @JsonProperty("Interface")
    public StorageControllerInterface controllerInterface;
    @JsonProperty("CapacityGiB")
    public Float capacityGib;
    public MediaType type;
    @JsonProperty("RPM")
    public Integer rpm;
    public String manufacturer;
    public String model;
    public String serialNumber;
    public Status status;
    public final Links links = new Links();

    public PhysicalDriveJson() {
        super("#PhysicalDrive.v1_0_0.PhysicalDrive");
    }

    @JsonPropertyOrder({"usedBy", "oem"})
    public class Links extends RedfishLinksJson {
        public Set<ODataId> usedBy = new HashSet<>();
    }
}
