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

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"data", "metadata", "snapshot", "isThinProvisioned"})
@JsonInclude(NON_NULL)
public class CapacityDto {
    private CapacityInfoDto data;
    private CapacityInfoDto metadata;
    private CapacityInfoDto snapshot;
    private Boolean isThinProvisioned;

    public CapacityInfoDto getData() {
        return data;
    }

    public void setData(CapacityInfoDto data) {
        this.data = data;
    }

    public CapacityInfoDto getMetadata() {
        return metadata;
    }

    public void setMetadata(CapacityInfoDto metadata) {
        this.metadata = metadata;
    }

    public CapacityInfoDto getSnapshot() {
        return snapshot;
    }

    public void setSnapshot(CapacityInfoDto snapshot) {
        this.snapshot = snapshot;
    }

    public Boolean getThinProvisioned() {
        return isThinProvisioned;
    }

    public void setThinProvisioned(Boolean thinProvisioned) {
        isThinProvisioned = thinProvisioned;
    }
}
