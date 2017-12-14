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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.redfish.OemType;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;

@OdataTypes({
    "#PortMetrics" + VERSION_PATTERN + "PortMetrics"
})
public class PortMetricsResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Health")
    private String health;
    @JsonProperty("Actions")
    private Actions actions = new Actions();

    public String getHealth() {
        return health;
    }

    public Actions getActions() {
        return actions;
    }

    public class Actions extends RedfishActions {
        @OemType(OEM_IN_ACTIONS)
        public class Oem {
        }
    }
}
