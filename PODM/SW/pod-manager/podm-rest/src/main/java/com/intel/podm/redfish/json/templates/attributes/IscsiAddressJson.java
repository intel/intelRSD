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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.rest.odataid.ODataId;

import java.util.List;

@JsonPropertyOrder({"TargetLUN", "TargetIQN", "TargetPortalIP", "TargetPortalPort"})
@JsonInclude(JsonInclude.Include.NON_NULL)
public class IscsiAddressJson {

    @JsonProperty("TargetLUN")
    public List<TargetLun> targetLun;

    @JsonProperty("TargetIQN")
    public String targetIqn;

    @JsonProperty("TargetPortalIP")
    public String targetPortalIp;

    public int targetPortalPort;

    public static final class TargetLun {
        @JsonProperty("LUN")
        public Integer value;
        @JsonProperty("LogicalDrive")
        public ODataId drive;

        public TargetLun(Integer value, ODataId drive) {
            this.value = value;
            this.drive = drive;
        }
    }
}
