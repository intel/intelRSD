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
import com.intel.podm.common.types.redfish.RedfishEthernetSwitch;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public class EthernetSwitchPartialRepresentation implements RedfishEthernetSwitch {

    @JsonProperty("LLDPEnabled")
    private Boolean lldpEnabled;

    @JsonProperty("ETSEnabled")
    private Boolean etsEnabled;

    @JsonProperty("DCBXEnabled")
    private Boolean dcbxEnabled;

    @JsonProperty("PFCEnabled")
    private Boolean pfcEnabled;

    @JsonProperty("DCBXSharedConfiguration")
    private DdbxConfigurationPartialRepresentation dcbxSharedConfiguration;


    @Override
    public Boolean getLldpEnabled() {
        return lldpEnabled;
    }

    @Override
    public Boolean getEtsEnabled() {
        return etsEnabled;
    }

    @Override
    public Boolean getDcbxEnabled() {
        return dcbxEnabled;
    }

    @Override
    public Boolean getPfcEnabled() {
        return pfcEnabled;
    }

    @Override
    public DcbxConfiguration getDcbxSharedConfiguration() {
        return dcbxSharedConfiguration;
    }
}
