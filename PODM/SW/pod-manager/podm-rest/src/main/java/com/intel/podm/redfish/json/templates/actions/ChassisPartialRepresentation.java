/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishChassis;

public final class ChassisPartialRepresentation implements RedfishChassis {
    @JsonProperty("AssetTag")
    private String assetTag;

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    @Override
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public String getLocationId() {
        return oem.rackScaleOem.location.id;
    }

    @Override
    public String getGeoTag() {
        return oem.rackScaleOem.geoTag;
    }

    public static class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public static class RackScaleOem {
            @JsonProperty("Location")
            private Location location = new Location();

            @JsonProperty("GeoTag")
            private String geoTag;

            public static class Location {
                @JsonProperty("Id")
                private String id;
            }
        }
    }
}
