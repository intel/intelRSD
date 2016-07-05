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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import com.intel.podm.rest.odataid.ODataId;

import java.util.Collection;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "switchId", "name", "description", "manufacturer", "model",
        "manufacturingDate", "serialNumber", "partNumber", "firmwareName", "firmwareVersion", "role", "status", "oem",
        "ports", "links",
})
public class EthernetSwitchJson extends BaseJson {
    public Id id;
    public String switchId;
    public String name;
    public String description;
    public String manufacturer;
    public String model;
    public String manufacturingDate;
    public String serialNumber;
    public String partNumber;
    public String firmwareName;
    public String firmwareVersion;
    public String role;
    public Status status;
    public Object oem = new Object();
    public ODataId ports;
    public Links links = new Links();

    public EthernetSwitchJson() {
        super("#EthernetSwitch.1.0.0.EthernetSwitch");
    }

    @JsonPropertyOrder({"chassis", "managedBy", "oem"})
    public static class Links {
        public ODataId chassis;
        public Collection<ODataId> managedBy;
        public Object oem = new Object();
    }
}
