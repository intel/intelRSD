/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.redfish.MemoryChunkResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.AddressRangeType;
import com.intel.podm.common.types.Status;

@OdataTypes("#MemoryChunk.1.0.0.MemoryChunk")
public class MemoryChunkResourceImpl extends ExternalServiceResourceImpl implements MemoryChunkResource {
    @JsonProperty("MemoryChunkName")
    private String memoryChunkName;
    @JsonProperty("MemoryChunkUID")
    private Integer memoryChunkUid;
    @JsonProperty("MemoryChunkSizeMiB")
    private Integer memoryChunkSizeMib;
    @JsonProperty("AddressRangeType")
    private AddressRangeType addressRangeType;
    @JsonProperty("IsMirrorEnabled")
    private Boolean isMirrorEnabled;
    @JsonProperty("IsSpare")
    private Boolean isSpare;
    @JsonProperty("Status")
    private Status status;

    @Override
    public String getMemoryChunkName() {
        return memoryChunkName;
    }

    @Override
    public Integer getMemoryChunkUid() {
        return memoryChunkUid;
    }

    @Override
    public Integer getMemoryChunkSizeMib() {
        return memoryChunkSizeMib;
    }

    @Override
    public AddressRangeType getAddressRangeType() {
        return addressRangeType;
    }

    @Override
    public Boolean getIsMirrorEnabled() {
        return isMirrorEnabled;
    }

    @Override
    public Boolean getIsSpare() {
        return isSpare;
    }

    @Override
    public Status getStatus() {
        return status;
    }
}
