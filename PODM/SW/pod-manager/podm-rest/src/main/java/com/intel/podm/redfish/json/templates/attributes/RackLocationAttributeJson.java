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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.rmm.RackUnitType;

/**
 * Class for representing in JSON module location in a rack (e.g. thermal and power module)
 */
@JsonPropertyOrder({"uLocation", "uHeight", "xLocation", "rackUnits"})
public final class RackLocationAttributeJson {
    public final Integer uLocation;
    public final Integer uHeight;
    public final Integer xLocation;
    public final RackUnitType rackUnits;

    public RackLocationAttributeJson(Integer uLocation, Integer uHeight, Integer xLocation, RackUnitType unit) {
        this.uLocation = uLocation;
        this.uHeight = uHeight;
        this.xLocation = xLocation;
        this.rackUnits = unit;
    }
}
