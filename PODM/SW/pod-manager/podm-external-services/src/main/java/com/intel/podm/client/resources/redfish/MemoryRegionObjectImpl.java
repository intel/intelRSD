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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.api.resources.redfish.MemoryRegionObject;
import com.intel.podm.common.types.MemoryClassification;

@JsonIgnoreProperties(ignoreUnknown = true)
public class MemoryRegionObjectImpl implements MemoryRegionObject {
    @JsonProperty("RegionId")
    private String regionId;

    @JsonProperty("MemoryClassification")
    private MemoryClassification memoryClassification;

    @JsonProperty("OffsetMiB")
    private Integer offsetMib;

    @JsonProperty("SizeMiB")
    private Integer sizeMib;

    @Override
    public String getRegionId() {
        return regionId;
    }

    @Override
    public MemoryClassification getMemoryClassification() {
        return memoryClassification;
    }

    @Override
    public Integer getOffsetMib() {
        return offsetMib;
    }

    @Override
    public Integer getSizeMib() {
        return sizeMib;
    }
}
