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
import com.intel.podm.common.types.AddressRangeType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.InterleaveSetJson;

import java.util.List;

import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "memoryChunkName", "memoryChunkUid",
        "memoryChunkSizeMib", "addressRangeType", "isMirrorEnabled", "isSpare", "status", "interleaveSets", "oem"})

public class MemoryChunkJson extends BaseJson {
    public String name;
    public String description;
    public Id id;
    public String memoryChunkName;
    @JsonProperty("MemoryChunkUID")
    public Integer memoryChunkUid;
    @JsonProperty("MemoryChunkSizeMiB")
    public Integer memoryChunkSizeMib;
    public AddressRangeType addressRangeType;
    public Boolean isMirrorEnabled;
    public Boolean isSpare;
    public List<InterleaveSetJson> interleaveSets = newArrayList();
    public Status status;
    public Oem oem = new Oem();

    public MemoryChunkJson() {
        super("#MemoryChunk.1.0.0.MemoryChunk");
    }

    private static class Oem {
    }
}
