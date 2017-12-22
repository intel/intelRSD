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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.actions.BindActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({"@odata.type", "id", "name", "description", "oem", "rules", "links", "actions"})
public class EthernetSwitchAclDto extends RedfishDto {
    private SingletonContext rules;
    private final Links links = new Links();
    private final Actions actions = new Actions();

    public EthernetSwitchAclDto() {
        super("#EthernetSwitchACL.v1_0_0.EthernetSwitchACL");
    }

    public SingletonContext getRules() {
        return rules;
    }

    public void setRules(SingletonContext rules) {
        this.rules = rules;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    public final class Links extends RedfishLinksDto {
        private final Set<Context> boundPorts = new HashSet<>();

        public Set<Context> getBoundPorts() {
            return boundPorts;
        }
    }

    @JsonPropertyOrder({"bind", "unbind", "oem"})
    public final class Actions extends RedfishActionsDto {
        @JsonProperty("#EthernetSwitchACL.Bind")
        private final BindActionDto bind = new BindActionDto();
        @JsonProperty("#EthernetSwitchACL.Unbind")
        private final BindActionDto unbind = new BindActionDto();

        public BindActionDto getBind() {
            return bind;
        }

        public BindActionDto getUnbind() {
            return unbind;
        }
    }
}
