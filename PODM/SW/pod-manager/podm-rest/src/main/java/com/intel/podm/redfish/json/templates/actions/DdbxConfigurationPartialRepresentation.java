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

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.RedfishEthernetSwitch.DcbxConfiguration;

import java.util.List;

@JsonInclude(NON_NULL)
public class DdbxConfigurationPartialRepresentation implements DcbxConfiguration {

    @JsonProperty("ApplicationProtocol")
    private List<ApplicationProtocolTypePartialRepresentation> applicationProtocolList;

    @JsonProperty("PriorityToPriorityGroupMapping")
    private List<PriorityClassMappingPartialRepresentation> priorityToPriorityGroupMappingList;

    @JsonProperty("BandwidthAllocation")
    private List<BandwidthMappingPartialRepresentation> bandwidthAllocationList;


    @Override
    public List<? extends ApplicationProtocolType> getApplicationProtocolList() {
        return applicationProtocolList;
    }

    @Override
    public List<? extends PriorityClassMapping> getPriorityToPriorityGroupMappingList() {
        return priorityToPriorityGroupMappingList;
    }

    @Override
    public List<? extends BandwidthMapping> getBandwidthAllocationList() {
        return bandwidthAllocationList;
    }
}
