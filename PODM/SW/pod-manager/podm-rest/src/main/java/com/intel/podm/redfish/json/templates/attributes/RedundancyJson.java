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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.common.types.Status;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
        "oDataId", "memberId", "name", "redundancyEnabled", "redundancySet", "mode",
        "status", "minNumNeeded", "maxNumSupported"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class RedundancyJson {
    @JsonProperty("@odata.id")
    @JsonUnwrapped
    public ODataId oDataId;
    public String memberId;
    public String name;
    public Boolean redundancyEnabled;
    public Set<ODataId> redundancySet = new HashSet<>();
    public String mode;
    public Status status;
    public Integer minNumNeeded;
    public Integer maxNumSupported;
}
