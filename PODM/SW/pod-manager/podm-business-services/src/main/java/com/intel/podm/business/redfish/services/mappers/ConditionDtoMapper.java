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

import com.intel.podm.business.dto.EthernetSwitchAclRuleDto.ConditionDto;
import com.intel.podm.business.dto.EthernetSwitchAclRuleDto.ConditionDto.IpConditionDto;
import com.intel.podm.business.dto.EthernetSwitchAclRuleDto.ConditionDto.MacConditionDto;
import com.intel.podm.business.dto.EthernetSwitchAclRuleDto.ConditionDto.PortConditionDto;
import com.intel.podm.business.dto.EthernetSwitchAclRuleDto.ConditionDto.VlanIdConditionDto;
import com.intel.podm.business.entities.redfish.embeddables.Condition;
import com.intel.podm.business.entities.redfish.embeddables.Condition.IpCondition;
import com.intel.podm.business.entities.redfish.embeddables.Condition.MacCondition;
import com.intel.podm.business.entities.redfish.embeddables.Condition.PortCondition;
import com.intel.podm.business.entities.redfish.embeddables.Condition.VlanIdCondition;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
@SuppressWarnings("checkstyle:ClassFanoutComplexity")
class ConditionDtoMapper extends BrilliantMapper<Condition, ConditionDto> {
    @Inject
    private IpConditionDtoMapper ipConditionDtoMapper;

    @Inject
    private MacConditionDtoMapper macConditionDtoMapper;

    @Inject
    private VlanIdConditionDtoMapper vlanIdConditionDtoMapper;

    @Inject
    private PortConditionDtoMapper portConditionDtoMapper;

    ConditionDtoMapper() {
        super(Condition.class, ConditionDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(Condition source, ConditionDto target) {
        super.performNotAutomatedMapping(source, target);

        mapIpCondition(source, target);
        mapMacCondition(source, target);
        mapVlanIdCondition(source, target);
        mapPortCondition(source, target);
    }

    private void mapPortCondition(Condition source, ConditionDto target) {
        PortCondition portSource = source.getL4SourcePort();
        if (portSource != null) {
            target.setL4SourcePort(new PortConditionDto());
            portConditionDtoMapper.map(portSource, target.getL4SourcePort());
        }

        PortCondition portDestination = source.getL4DestinationPort();
        if (portDestination != null) {
            target.setL4DestinationPort(new PortConditionDto());
            portConditionDtoMapper.map(portDestination, target.getL4DestinationPort());
        }
    }

    private void mapVlanIdCondition(Condition source, ConditionDto target) {
        VlanIdCondition vlanIdCondition = source.getVlanId();
        if (vlanIdCondition != null) {
            target.setVlanId(new VlanIdConditionDto());
            vlanIdConditionDtoMapper.map(vlanIdCondition, target.getVlanId());
        }
    }

    private void mapMacCondition(Condition source, ConditionDto target) {
        MacCondition macSource = source.getMacSource();
        if (macSource != null) {
            target.setMacSource(new MacConditionDto());
            macConditionDtoMapper.map(macSource, target.getMacSource());
        }

        MacCondition macDestination = source.getMacDestination();
        if (macDestination != null) {
            target.setMacDestination(new MacConditionDto());
            macConditionDtoMapper.map(macDestination, target.getMacDestination());
        }
    }

    private void mapIpCondition(Condition source, ConditionDto target) {
        IpCondition ipSource = source.getIpSource();
        if (ipSource != null) {
            target.setIpSource(new IpConditionDto());
            ipConditionDtoMapper.map(ipSource, target.getIpSource());
        }

        IpCondition ipDestination = source.getIpDestination();
        if (ipDestination != null) {
            target.setIpDestination(new IpConditionDto());
            ipConditionDtoMapper.map(ipDestination, target.getIpDestination());
        }
    }

    @Dependent
    public static class IpConditionDtoMapper extends BrilliantMapper<IpCondition, IpConditionDto> {
        protected IpConditionDtoMapper() {
            super(IpCondition.class, IpConditionDto.class);
        }
    }

    @Dependent
    public static class MacConditionDtoMapper extends BrilliantMapper<MacCondition, MacConditionDto> {
        MacConditionDtoMapper() {
            super(MacCondition.class, MacConditionDto.class);
        }
    }

    @Dependent
    public static class VlanIdConditionDtoMapper extends BrilliantMapper<VlanIdCondition, VlanIdConditionDto> {
        VlanIdConditionDtoMapper() {
            super(VlanIdCondition.class, VlanIdConditionDto.class);
        }
    }

    @Dependent
    public static class PortConditionDtoMapper extends BrilliantMapper<PortCondition, PortConditionDto> {
        PortConditionDtoMapper() {
            super(PortCondition.class, PortConditionDto.class);
        }
    }
}
