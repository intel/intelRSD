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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.EthernetSwitchAclRuleDto;
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.embeddables.Condition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchPortContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
class EthernetSwitchAclRuleDtoMapper extends DtoMapper<EthernetSwitchAclRule, EthernetSwitchAclRuleDto> {
    @Inject
    private ConditionDtoMapper conditionDtoMapper;

    EthernetSwitchAclRuleDtoMapper() {
        super(EthernetSwitchAclRule.class, EthernetSwitchAclRuleDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchAclRule source, EthernetSwitchAclRuleDto target) {
        super.performNotAutomatedMapping(source, target);

        target.setForwardMirrorInterface(toContext(source.getForwardMirrorInterface()));
        target.getMirrorPortRegion().addAll(asEthernetSwitchPortContexts(source.getMirrorPortRegions()));

        final Condition sourceCondition = source.getCondition();
        if (sourceCondition != null) {
            conditionDtoMapper.map(sourceCondition, target.getCondition());
        }
    }
}
