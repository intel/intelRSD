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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "status", "type", "mode", "writeProtected", "assetTag", "capacityGib",
    "image", "bootable", "snapshot", "links"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public final class LogicalDriveJson extends BaseResourceJson {
    public LogicalDriveType type;
    public VolumeMode mode;
    @JsonProperty("Protected")
    public Boolean writeProtected;
    public Boolean snapshot;
    @JsonProperty("CapacityGiB")
    public BigDecimal capacityGib;
    public String image;
    public Boolean bootable;
    public Status status;
    public final Links links = new Links();

    public LogicalDriveJson() {
        super("#LogicalDrive.v1_0_0.LogicalDrive");
    }

    @JsonPropertyOrder({
      "logicalDrives", "physicalDrives", "masterDrive", "usedBy", "targets", "pcieFunctions"
    })
    @JsonInclude(NON_NULL)
    public class Links extends RedfishLinksJson {
        public Set<ODataId> logicalDrives = new HashSet<>();
        public Set<ODataId> physicalDrives = new HashSet<>();
        public ODataId masterDrive;
        public Set<ODataId> usedBy = new HashSet<>();
        public Set<ODataId> targets = new HashSet<>();
    }
}
