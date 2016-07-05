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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LogicalDriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.VolumeMode;

import java.math.BigDecimal;
import java.util.Collection;

public final class LogicalDriveDto {
    private final Id id;
    private final String name;
    private final String description;
    private final Status status;
    private final LogicalDriveType type;
    private final VolumeMode mode;
    private final Boolean writeProtected;
    private final BigDecimal capacityGib;
    private final String image;
    private final Boolean bootable;
    private final Boolean snapshot;
    private final Context context;

    private final Collection<Context> logicalDrives;
    private final Collection<Context> physicalDrives;
    private final Context masterDrive;
    private final Collection<Context> usedBy;
    private final Collection<Context> targets;

    private LogicalDriveDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        status = builder.status;
        type = builder.type;
        mode = builder.mode;
        writeProtected = builder.writeProtected;
        capacityGib = builder.capacityGib;
        image = builder.image;
        bootable = builder.bootable;
        snapshot = builder.snapshot;
        context = builder.context;
        logicalDrives = builder.logicalDrives;
        physicalDrives = builder.physicalDrives;
        masterDrive = builder.masterDrive;
        usedBy = builder.usedBy;
        targets = builder.targets;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
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

    public Collection<Context> getLogicalDrives() {
        return logicalDrives;
    }

    public Collection<Context> getPhysicalDrives() {
        return physicalDrives;
    }

    public Context getMasterDrive() {
        return masterDrive;
    }

    public Collection<Context> getUsedBy() {
        return usedBy;
    }

    public Collection<Context> getTargets() {
        return targets;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private Status status;
        private LogicalDriveType type;
        private VolumeMode mode;
        private Boolean writeProtected;
        private BigDecimal capacityGib;
        private String image;
        private Boolean bootable;
        private Boolean snapshot;
        private Context context;
        private Collection<Context> logicalDrives;
        private Collection<Context> physicalDrives;
        private Context masterDrive;
        private Collection<Context> usedBy;
        private Collection<Context> targets;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder type(LogicalDriveType val) {
            type = val;
            return this;
        }

        public Builder mode(VolumeMode val) {
            mode = val;
            return this;
        }

        public Builder writeProtected(Boolean val) {
            writeProtected = val;
            return this;
        }

        public Builder capacityGib(BigDecimal val) {
            capacityGib = val;
            return this;
        }

        public Builder image(String val) {
            image = val;
            return this;
        }

        public Builder bootable(Boolean val) {
            bootable = val;
            return this;
        }

        public Builder snapshot(Boolean val) {
            snapshot = val;
            return this;
        }

        public Builder context(Context val) {
            context = val;
            return this;
        }

        public Builder logicalDrives(Collection<Context> val) {
            logicalDrives = val;
            return this;
        }

        public Builder physicalDrives(Collection<Context> val) {
            physicalDrives = val;
            return this;
        }

        public Builder masterDrive(Context val) {
            masterDrive = val;
            return this;
        }

        public Builder usedBy(Collection<Context> val) {
            usedBy = val;
            return this;
        }

        public Builder targets(Collection<Context> val) {
            targets = val;
            return this;
        }

        public LogicalDriveDto build() {
            return new LogicalDriveDto(this);
        }
    }
}
