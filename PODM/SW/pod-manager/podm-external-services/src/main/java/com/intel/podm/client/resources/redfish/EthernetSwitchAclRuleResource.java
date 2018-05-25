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
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;

import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#EthernetSwitchACLRule" + VERSION_PATTERN + "EthernetSwitchACLRule"
})
public class EthernetSwitchAclRuleResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("RuleId")
    private Long ruleId;
    @JsonProperty("Action")
    private ActionType action;
    @JsonProperty("ForwardMirrorInterface")
    private ODataId forwardMirrorInterface;
    @JsonProperty("MirrorPortRegion")
    private Set<ODataId> mirrorPortRegion;
    @JsonProperty("MirrorType")
    private MirrorType mirrorType;
    @JsonProperty("Condition")
    private ConditionObject condition = new ConditionObject();

    public Long getRuleId() {
        return ruleId;
    }

    public ActionType getAction() {
        return action;
    }

    public MirrorType getMirrorType() {
        return mirrorType;
    }

    public ConditionObject getCondition() {
        return condition;
    }

    @LinkName("forwardMirrorInterface")
    public ResourceSupplier getForwardMirrorInterface() throws WebClientRequestException {
        if (forwardMirrorInterface == null) {
            return null;
        }
        return toSupplier(forwardMirrorInterface);
    }

    @LinkName("mirrorPortRegion")
    public Iterable<ResourceSupplier> getMirrorPortRegion() throws WebClientRequestException {
        return toSuppliers(mirrorPortRegion);
    }
}
