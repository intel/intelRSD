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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.business.services.redfish.odataid.ODataId;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "fabricType", "maxZones", "status", "zones", "endpoints", "switches", "fabricType",
    "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class FabricJson extends BaseResourceJson {
    public Protocol fabricType;
    public Status status;
    public Integer maxZones;
    public ODataId zones;
    public ODataId endpoints;
    public ODataId switches;
    public Links links = new Links();
    public Actions actions = new Actions();

    public FabricJson() {
        super("#Fabric.v1_0_0.Fabric");
    }

    public class Links extends RedfishLinksJson {
    }

    public class Actions extends RedfishActionsJson {
    }
}
