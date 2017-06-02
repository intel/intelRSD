/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.common.types.MemoryClassification;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class Region {
    @Column(name = "region_id")
    private String regionId;

    @Column(name = "memory_type")
    @Enumerated(STRING)
    private MemoryClassification memoryClassification;

    @Column(name = "offset_mib")
    private Integer offsetMib;

    @Column(name = "size_mib")
    private Integer sizeMib;

    public String getRegionId() {
        return regionId;
    }

    public void setRegionId(String regionId) {
        this.regionId = regionId;
    }

    public MemoryClassification getMemoryClassification() {
        return memoryClassification;
    }

    public void setMemoryClassification(MemoryClassification memoryClassification) {
        this.memoryClassification = memoryClassification;
    }

    public Integer getOffsetMib() {
        return offsetMib;
    }

    public void setOffsetMib(Integer offsetMib) {
        this.offsetMib = offsetMib;
    }

    public Integer getSizeMib() {
        return sizeMib;
    }

    public void setSizeMib(Integer sizeMib) {
        this.sizeMib = sizeMib;
    }

    @Override
    @SuppressWarnings("checkstyle:CyclomaticComplexity")
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        Region region = (Region) o;
        return Objects.equals(regionId, region.regionId)
            && memoryClassification == region.memoryClassification
            && Objects.equals(offsetMib, region.offsetMib)
            && Objects.equals(sizeMib, region.sizeMib);
    }

    @Override
    public int hashCode() {
        return Objects.hash(regionId, memoryClassification, offsetMib, sizeMib);
    }
}
