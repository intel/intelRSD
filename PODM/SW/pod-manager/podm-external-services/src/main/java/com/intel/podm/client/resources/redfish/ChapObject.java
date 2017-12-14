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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.common.types.ChapType;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#RemoteTarget" + VERSION_PATTERN + "Chap"
})
@JsonIgnoreProperties(ignoreUnknown = true)
public class ChapObject {
    @JsonProperty("Type")
    private ChapType type;

    @JsonProperty("Username")
    private String username;

    @JsonProperty("MutualUsername")
    private String mutualUsername;

    public ChapType getType() {
        return type;
    }

    public String getUsername() {
        return username;
    }

    public String getMutualUsername() {
        return mutualUsername;
    }
}
