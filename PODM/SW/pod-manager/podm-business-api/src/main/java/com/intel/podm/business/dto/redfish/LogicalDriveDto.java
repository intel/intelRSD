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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.math.BigDecimal;
import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class LogicalDriveDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final LogicalDriveType type;
    private final VolumeMode mode;
    private final Boolean writeProtected;
    private final BigDecimal capacityGib;
    private final String image;
    private final Boolean bootable;
    private final Boolean snapshot;
    private final Context context;
    private final Links links;

    private LogicalDriveDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        type = builder.type;
        mode = builder.mode;
        writeProtected = builder.writeProtected;
        capacityGib = builder.capacityGib;
        image = builder.image;
        bootable = builder.bootable;
        snapshot = builder.snapshot;
        context = builder.context;
        links = Links.newBuilder()
            .logicalDrives(builder.logicalDrives)
            .physicalDrives(builder.physicalDrives)
            .masterDrive(builder.masterDrive)
            .usedBy(builder.usedBy)
            .targets(builder.targets)
            .build();
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public Status getStatus() {
        return status;
    }

    public LogicalDriveType getType() {
        return type;
    }

    public VolumeMode getMode() {
        return mode;
    }

    public Boolean getWriteProtected() {
        return writeProtected;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public String getImage() {
        return image;
    }

    public Boolean getBootable() {
        return bootable;
    }

    public Boolean getSnapshot() {
        return snapshot;
    }

    public Context getContext() {
        return context;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> logicalDrives;
        private final Set<Context> physicalDrives;
        private final Context masterDrive;
        private final Set<Context> usedBy;
        private final Set<Context> targets;

        private Links(Builder builder) {
            logicalDrives = builder.logicalDrives;
            physicalDrives = builder.physicalDrives;
            masterDrive = builder.masterDrive;
            usedBy = builder.usedBy;
            targets = builder.targets;
        }

        public static Builder newBuilder() {
            return new Builder();
        }

        public Set<Context> getLogicalDrives() {
            return logicalDrives;
        }

        public Set<Context> getPhysicalDrives() {
            return physicalDrives;
        }

        public Context getMasterDrive() {
            return masterDrive;
        }

        public Set<Context> getUsedBy() {
            return usedBy;
        }

        public Set<Context> getTargets() {
            return targets;
        }

        public static final class Builder {
            private Set<Context> logicalDrives;
            private Set<Context> physicalDrives;
            private Context masterDrive;
            private Set<Context> usedBy;
            private Set<Context> targets;

            private Builder() {
            }

            public Builder logicalDrives(Set<Context> logicalDrives) {
                this.logicalDrives = logicalDrives;
                return this;
            }

            public Builder physicalDrives(Set<Context> physicalDrives) {
                this.physicalDrives = physicalDrives;
                return this;
            }

            public Builder masterDrive(Context masterDrive) {
                this.masterDrive = masterDrive;
                return this;
            }

            public Builder usedBy(Set<Context> usedBy) {
                this.usedBy = usedBy;
                return this;
            }

            public Builder targets(Set<Context> targets) {
                this.targets = targets;
                return this;
            }

            public Links build() {
                return new Links(this);
            }
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private LogicalDriveType type;
        private VolumeMode mode;
        private Boolean writeProtected;
        private BigDecimal capacityGib;
        private String image;
        private Boolean bootable;
        private Boolean snapshot;
        private Context context;
        private Set<Context> logicalDrives;
        private Set<Context> physicalDrives;
        private Context masterDrive;
        private Set<Context> usedBy;
        private Set<Context> targets;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder type(LogicalDriveType type) {
            this.type = type;
            return this;
        }

        public Builder mode(VolumeMode mode) {
            this.mode = mode;
            return this;
        }

        public Builder writeProtected(Boolean writeProtected) {
            this.writeProtected = writeProtected;
            return this;
        }

        public Builder capacityGib(BigDecimal capacityGib) {
            this.capacityGib = capacityGib;
            return this;
        }

        public Builder image(String image) {
            this.image = image;
            return this;
        }

        public Builder bootable(Boolean bootable) {
            this.bootable = bootable;
            return this;
        }

        public Builder snapshot(Boolean snapshot) {
            this.snapshot = snapshot;
            return this;
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }

        public Builder logicalDrives(Set<Context> logicalDrives) {
            this.logicalDrives = logicalDrives;
            return this;
        }

        public Builder physicalDrives(Set<Context> physicalDrives) {
            this.physicalDrives = physicalDrives;
            return this;
        }

        public Builder masterDrive(Context masterDrive) {
            this.masterDrive = masterDrive;
            return this;
        }

        public Builder usedBy(Set<Context> usedBy) {
            this.usedBy = usedBy;
            return this;
        }

        public Builder targets(Set<Context> targets) {
            this.targets = targets;
            return this;
        }

        public LogicalDriveDto build() {
            return new LogicalDriveDto(this);
        }
    }
}
