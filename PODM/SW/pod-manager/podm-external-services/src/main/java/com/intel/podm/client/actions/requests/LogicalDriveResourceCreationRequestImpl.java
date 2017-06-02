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

package com.intel.podm.client.actions.requests;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.api.actions.LogicalDriveResourceCreationRequest;
import com.intel.podm.common.types.VolumeMode;
import com.intel.podm.common.types.VolumeType;

import java.math.BigDecimal;
import java.net.URI;

import static com.intel.podm.common.types.VolumeMode.LV;
import static com.intel.podm.common.types.VolumeType.LVM;

@JsonPropertyOrder({"Name", "Type", "Mode", "Protected", "CapacityGiB", "Bootable", "Snapshot", "Links"})
public class LogicalDriveResourceCreationRequestImpl implements LogicalDriveResourceCreationRequest {
    @JsonProperty("Name")
    private final String name = "Logical Drive";

    @JsonProperty("Type")
    private final VolumeType type = LVM;

    @JsonProperty("Mode")
    private final VolumeMode mode = LV;

    @JsonProperty("Protected")
    private final boolean protection = false;

    @JsonProperty("Bootable")
    private final boolean bootable = true;

    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;

    @JsonProperty("Snapshot")
    private boolean snapshot;

    @JsonProperty("Links")
    private final RequestLinks links = new RequestLinks();

    @Override
    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    @Override
    public void setSnapshot(boolean isSnapshot) {
        snapshot = isSnapshot;
    }

    @Override
    public void setMaster(URI master) {
        links.setMasterDrive(master);
    }

    @Override
    public void setLvg(URI lvg) {
        links.setLogicalDrive(lvg);
    }
}
