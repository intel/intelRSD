/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.common.types.Status;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.Set;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "switchId", "name", "description", "manufacturer", "model",
    "manufacturingDate", "serialNumber", "partNumber", "firmwareName", "firmwareVersion", "role", "status", "oem",
    "ports", "links",
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class EthernetSwitchJson extends BaseResourceJson {
    public String switchId;
    public String manufacturer;
    public String model;
    public String manufacturingDate;
    public String serialNumber;
    public String partNumber;
    public String firmwareName;
    public String firmwareVersion;
    public String role;
    public Status status;
    public ODataId ports;
    public Links links = new Links();

    public EthernetSwitchJson() {
        super("#EthernetSwitch.v1_0_0.EthernetSwitch");
    }

    @JsonPropertyOrder({"chassis", "pcieFunctions", "oem"})
    public class Links extends RedfishLinksJson {
        public ODataId chassis;
        public Set<ODataId> managedBy;
    }
}
