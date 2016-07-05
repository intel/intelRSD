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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;
import com.intel.podm.rest.odataid.ODataId;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "storageControllerInterface",
        "manufacturer", "model", "status", "busInfo", "oem", "devices", "links"
})
public class AdapterJson extends BaseJson {
    public String id;
    public String name;
    public String description;
    @JsonProperty("Interface")
    public StorageControllerInterface storageControllerInterface;
    public String manufacturer;
    public String model;
    public Status status;
    public String busInfo;
    public ODataId devices;
    public Object oem = new Object();
    public Links links = new Links();

    public AdapterJson() {
        super("#Adapter.1.0.0.Adapter");
    }

    public static class Links {
        public ODataId containedBy;
        public Object oem = new Object();
    }
}
