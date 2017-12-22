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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.rmm.RackUnitType;

@JsonPropertyOrder({"uLocation", "uHeight", "xLocation", "rackUnits"})
public final class RackLocationDto {
    private Integer uLocation;
    private Integer uHeight;
    private Integer xLocation;
    private RackUnitType rackUnits;

    public Integer getuLocation() {
        return uLocation;
    }

    public void setuLocation(Integer uLocation) {
        this.uLocation = uLocation;
    }

    public Integer getuHeight() {
        return uHeight;
    }

    public void setuHeight(Integer uHeight) {
        this.uHeight = uHeight;
    }

    public Integer getxLocation() {
        return xLocation;
    }

    public void setxLocation(Integer xLocation) {
        this.xLocation = xLocation;
    }

    public RackUnitType getRackUnits() {
        return rackUnits;
    }

    public void setRackUnits(RackUnitType rackUnits) {
        this.rackUnits = rackUnits;
    }
}
