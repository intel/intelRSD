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
import com.intel.podm.business.services.context.Context;

import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"providedCapacity", "providingVolumes", "providingPools", "providingDrives"})
@JsonInclude(NON_NULL)
public class CapacitySourceDto {
    private CapacityDto providedCapacity;
    private Set<Context> providingVolumes = new HashSet<>();
    private Set<Context> providingPools = new HashSet<>();
    private Set<Context> providingDrives = new HashSet<>();

    public CapacityDto getProvidedCapacity() {
        return providedCapacity;
    }

    public void setProvidedCapacity(CapacityDto providedCapacity) {
        this.providedCapacity = providedCapacity;
    }

    public Set<Context> getProvidingVolumes() {
        return providingVolumes;
    }

    public void setProvidingVolumes(Set<Context> providingVolumes) {
        this.providingVolumes = providingVolumes;
    }

    public Set<Context> getProvidingPools() {
        return providingPools;
    }

    public void setProvidingPools(Set<Context> providingPools) {
        this.providingPools = providingPools;
    }

    public Set<Context> getProvidingDrives() {
        return providingDrives;
    }

    public void setProvidingDrives(Set<Context> providingDrives) {
        this.providingDrives = providingDrives;
    }
}
