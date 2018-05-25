/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Embedded;

import java.util.Objects;

@Embeddable
public class Capacity {
    @Column(name = "is_thin_provisioned")
    private Boolean isThinProvisioned;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "consumedBytes", column = @Column(name = "data_consumed_bytes")),
        @AttributeOverride(name = "allocatedBytes", column = @Column(name = "data_allocated_bytes")),
        @AttributeOverride(name = "guaranteedBytes", column = @Column(name = "data_guaranteed_bytes")),
        @AttributeOverride(name = "provisionedBytes", column = @Column(name = "data_provisioned_bytes"))
    })
    private CapacityInfo data;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "consumedBytes", column = @Column(name = "metadata_consumed_bytes")),
        @AttributeOverride(name = "allocatedBytes", column = @Column(name = "metadata_allocated_bytes")),
        @AttributeOverride(name = "guaranteedBytes", column = @Column(name = "metadata_guaranteed_bytes")),
        @AttributeOverride(name = "provisionedBytes", column = @Column(name = "metadata_provisioned_bytes"))
    })
    private CapacityInfo metadata;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "consumedBytes", column = @Column(name = "snapshot_consumed_bytes")),
        @AttributeOverride(name = "allocatedBytes", column = @Column(name = "snapshot_allocated_bytes")),
        @AttributeOverride(name = "guaranteedBytes", column = @Column(name = "snapshot_guaranteed_bytes")),
        @AttributeOverride(name = "provisionedBytes", column = @Column(name = "snapshot_provisioned_bytes"))
    })
    private CapacityInfo snapshot;

    public Boolean getThinProvisioned() {
        return isThinProvisioned;
    }

    public void setThinProvisioned(Boolean thinProvisioned) {
        isThinProvisioned = thinProvisioned;
    }

    public CapacityInfo getData() {
        return data;
    }

    public void setData(CapacityInfo data) {
        this.data = data;
    }

    public CapacityInfo getMetadata() {
        return metadata;
    }

    public void setMetadata(CapacityInfo metadata) {
        this.metadata = metadata;
    }

    public CapacityInfo getSnapshot() {
        return snapshot;
    }

    public void setSnapshot(CapacityInfo snapshot) {
        this.snapshot = snapshot;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Capacity capacity = (Capacity) o;

        return new EqualsBuilder()
            .append(isThinProvisioned, capacity.isThinProvisioned)
            .append(data, capacity.data)
            .append(metadata, capacity.metadata)
            .append(snapshot, capacity.snapshot)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(isThinProvisioned, data, metadata, snapshot);
    }
}
