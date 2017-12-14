/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.common.types.redfish.OemType;

import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "health"
})
public class PortMetricsDto extends RedfishDto {
    @JsonProperty("Health")
    private String health;
    @JsonProperty("Actions")
    private final Actions actions = new Actions();

    public PortMetricsDto() {
        super("#PortMetrics.v1_0_0.PortMetrics");
    }

    public String getHealth() {
        return health;
    }

    public void setHealth(String health) {
        this.health = health;
    }

    public Actions getActions() {
        return actions;
    }

    public final class Actions extends RedfishActionsDto {
        @OemType(OEM_IN_ACTIONS)
        public class Oem {
        }
    }
}
