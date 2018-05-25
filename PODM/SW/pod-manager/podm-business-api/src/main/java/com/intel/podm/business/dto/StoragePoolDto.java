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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.redfish.CapacityDto;
import com.intel.podm.business.dto.redfish.CapacitySourceDto;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "blockSizeBytes", "status", "identifier", "allocatedVolumes", "allocatedPools",
    "capacity", "capacitySources", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class StoragePoolDto extends RedfishDto {
    private final Links links = new Links();
    private DurableIdentifierDto identifier;
    private BigDecimal blockSizeBytes;
    private CapacityDto capacity;
    private List<CapacitySourceDto> capacitySources = new ArrayList<>();
    private Status status;
    private SingletonContext allocatedVolumes;
    private SingletonContext allocatedPools;

    public StoragePoolDto() {
        super("#StoragePool.v1_0_0.StoragePool");
    }

    public DurableIdentifierDto getIdentifier() {
        return identifier;
    }

    public void setIdentifier(DurableIdentifierDto identifier) {
        this.identifier = identifier;
    }

    public BigDecimal getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(BigDecimal blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getAllocatedVolumes() {
        return allocatedVolumes;
    }

    public void setAllocatedVolumes(SingletonContext allocatedVolumes) {
        this.allocatedVolumes = allocatedVolumes;
    }

    public SingletonContext getAllocatedPools() {
        return allocatedPools;
    }

    public void setAllocatedPools(SingletonContext allocatedPools) {
        this.allocatedPools = allocatedPools;
    }

    public Links getLinks() {
        return links;
    }

    public CapacityDto getCapacity() {
        return capacity;
    }

    public void setCapacity(CapacityDto capacity) {
        this.capacity = capacity;
    }

    public List<CapacitySourceDto> getCapacitySources() {
        return capacitySources;
    }

    public void setCapacitySources(List<CapacitySourceDto> capacitySources) {
        this.capacitySources = capacitySources;
    }

    @JsonPropertyOrder({"@odata.type", "oem"})
    public final class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#StoragePool.v1_0_0.Links";

        public String getoDataType() {
            return oDataType;
        }
    }
}
