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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;

import java.math.BigDecimal;
import java.util.Collection;
import java.util.HashSet;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "status", "type", "mode", "writeProtected", "assetTag", "capacityGib",
    "image", "bootable", "snapshot", "links"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class LogicalDriveDto extends RedfishDto {
    private Status status;
    private LogicalDriveType type;
    private VolumeMode mode;
    @JsonProperty("Protected")
    private Boolean writeProtected;
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;
    private String image;
    private Boolean bootable;
    private Boolean snapshot;
    private final Links links = new Links();

    public LogicalDriveDto() {
        super("#LogicalDrive.v1_0_0.LogicalDrive");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public LogicalDriveType getType() {
        return type;
    }

    public void setType(LogicalDriveType type) {
        this.type = type;
    }

    public VolumeMode getMode() {
        return mode;
    }

    public void setMode(VolumeMode mode) {
        this.mode = mode;
    }

    public Boolean getWriteProtected() {
        return writeProtected;
    }

    public void setWriteProtected(Boolean writeProtected) {
        this.writeProtected = writeProtected;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    public String getImage() {
        return image;
    }

    public void setImage(String image) {
        this.image = image;
    }

    public Boolean getBootable() {
        return bootable;
    }

    public void setBootable(Boolean bootable) {
        this.bootable = bootable;
    }

    public Boolean getSnapshot() {
        return snapshot;
    }

    public void setSnapshot(Boolean snapshot) {
        this.snapshot = snapshot;
    }

    public Links getLinks() {
        return links;
    }

    @JsonPropertyOrder({
        "logicalDrives", "physicalDrives", "masterDrive", "usedBy", "targets", "pcieFunctions"
    })
    @JsonInclude(NON_NULL)
    public final class Links extends RedfishLinksDto {
        private final Collection<Context> logicalDrives = new HashSet<>();
        private final Collection<Context> physicalDrives = new HashSet<>();
        private Context masterDrive;
        private final Collection<Context> usedBy = new HashSet<>();
        private final Collection<Context> targets = new HashSet<>();

        public Collection<Context> getLogicalDrives() {
            return logicalDrives;
        }

        public Collection<Context> getPhysicalDrives() {
            return physicalDrives;
        }

        public Context getMasterDrive() {
            return masterDrive;
        }

        public void setMasterDrive(Context masterDrive) {
            this.masterDrive = masterDrive;
        }

        public Collection<Context> getUsedBy() {
            return usedBy;
        }

        public Collection<Context> getTargets() {
            return targets;
        }
    }
}
