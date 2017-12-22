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
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;

import java.math.BigDecimal;
import java.util.List;

import static com.google.common.base.MoreObjects.toStringHelper;
import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static java.util.Collections.emptyList;

@OdataTypes({
    "#LogicalDrive" + VERSION_PATTERN + "LogicalDrive"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class LogicalDriveResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Type")
    private LogicalDriveType type;
    @JsonProperty("Mode")
    private VolumeMode mode;
    @JsonProperty("Protected")
    private Boolean writeProtected;
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;
    @JsonProperty("Image")
    private String image;
    @JsonProperty("Bootable")
    private Boolean bootable;
    @JsonProperty("Snapshot")
    private Boolean snapshot;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Links")
    private Links links = new Links();

    public LogicalDriveType getType() {
        return type;
    }

    public VolumeMode getMode() {
        return mode;
    }

    public Boolean isWriteProtected() {
        return writeProtected;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public String getImage() {
        return image;
    }

    public Boolean isBootable() {
        return bootable;
    }

    public Boolean isSnapshot() {
        return snapshot;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("logicalDrives")
    public Iterable<ResourceSupplier> getLogicalDrives() throws WebClientRequestException {
        return toSuppliers(links.logicalDrives);
    }

    @LinkName("physicalDrives")
    public Iterable<ResourceSupplier> getPhysicalDrives() throws WebClientRequestException {
        return toSuppliers(links.physicalDrives);
    }

    @LinkName("masterDrive")
    public ResourceSupplier getMasterDrive() {
        if (links.masterDrive == null) {
            return null;
        }
        return toSupplier(links.masterDrive);
    }

    public Iterable<ResourceSupplier> getUsedBy() throws WebClientRequestException {
        return toSuppliers(links.usedBy);
    }

    public Iterable<ResourceSupplier> getTargets() throws WebClientRequestException {
        return toSuppliers(links.targets);
    }

    @Override
    public String toString() {
        return toStringHelper(this)
            .add("type", type)
            .add("mode", mode)
            .add("writeProtected", writeProtected)
            .add("capacityGib", capacityGib)
            .add("image", image)
            .add("bootable", bootable)
            .add("snapshot", snapshot)
            .add("status", status)
            .add("links", links)
            .toString();
    }

    public class Links extends RedfishLinks {
        @JsonProperty("LogicalDrives")
        private List<ODataId> logicalDrives = emptyList();
        @JsonProperty("PhysicalDrives")
        private List<ODataId> physicalDrives = emptyList();
        @JsonProperty("MasterDrive")
        private ODataId masterDrive;
        @JsonProperty("UsedBy")
        private List<ODataId> usedBy = emptyList();
        @JsonProperty("Targets")
        private List<ODataId> targets = emptyList();
    }
}
