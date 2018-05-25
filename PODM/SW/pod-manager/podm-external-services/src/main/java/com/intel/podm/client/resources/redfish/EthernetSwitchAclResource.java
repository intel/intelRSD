/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;

import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#EthernetSwitchACL" + VERSION_PATTERN + "EthernetSwitchACL"
})
public class EthernetSwitchAclResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Rules")
    private ODataId rules;
    @JsonProperty("Links")
    private Links links = new Links();
    @JsonProperty("Actions")
    private Actions actions = new Actions();

    @LinkName("rules")
    public Iterable<ResourceSupplier> getRules() throws WebClientRequestException {
        return processMembersListResource(rules);
    }

    @LinkName("boundPorts")
    public Iterable<ResourceSupplier> getBoundPorts() throws WebClientRequestException {
        return toSuppliers(links.boundPorts);
    }

    @LinkName("bindActionAllowableValues")
    public Iterable<ResourceSupplier> getBindActionAllowableValues() throws WebClientRequestException {
        return toSuppliers(actions.bind.allowableValues);
    }

    public static class BindAction {
        private ODataId target;
        @JsonProperty("Port@Redfish.AllowableValues")
        private Set<ODataId> allowableValues = new HashSet<>();
    }

    public class Links extends RedfishLinks {
        @JsonProperty("BoundPorts")
        private Set<ODataId> boundPorts;
    }

    public class Actions extends RedfishActions {
        @JsonProperty("#EthernetSwitchACL.Bind")
        private BindAction bind;
        @JsonProperty("#EthernetSwitchACL.Unbind")
        private BindAction unbind;
    }
}
