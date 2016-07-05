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

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
        "status", "remoteTargets", "logicalDrives", "drives", "oem", "links"
})
public class StorageServiceJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public Status status;

    public ODataId remoteTargets;
    public ODataId logicalDrives;
    public ODataId drives;
    public Links links = new Links();
    public Object oem = new Object();

    public StorageServiceJson() {
        super("#StorageService.1.0.0.StorageService");
    }

    @JsonPropertyOrder({"managedBy", "oem"})
    public static final class Links {
        public List<ODataId> managedBy = new ArrayList<>();
        public Object oem = new Object();
    }
}
