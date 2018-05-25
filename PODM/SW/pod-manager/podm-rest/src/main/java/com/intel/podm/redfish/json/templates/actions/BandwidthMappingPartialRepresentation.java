/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration.BandwidthMapping;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public class BandwidthMappingPartialRepresentation implements BandwidthMapping {

    @JsonProperty("PriorityGroup")
    private Long priorityGroup;

    @JsonProperty("BandwidthPercent")
    private Long bandwidthPercent;

    @Override
    public Long getPriorityGroup() {
        return priorityGroup;
    }

    @Override
    public Long getBandwidthPercent() {
        return bandwidthPercent;
    }
}
