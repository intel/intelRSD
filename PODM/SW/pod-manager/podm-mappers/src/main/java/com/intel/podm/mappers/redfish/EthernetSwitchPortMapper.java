/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.embeddables.PriorityFlowControl;
import com.intel.podm.client.resources.redfish.EthernetSwitchPortResource;
import com.intel.podm.client.resources.redfish.PriorityFlowControlObject;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.IpV4AddressMapper;
import com.intel.podm.mappers.subresources.IpV6AddressMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.Optional.ofNullable;

@Dependent
public class EthernetSwitchPortMapper extends EntityMapper<EthernetSwitchPortResource, EthernetSwitchPort> {
    @Inject
    private IpV4AddressMapper ipV4AddressMapper;
    @Inject
    private IpV6AddressMapper ipV6AddressMapper;
    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    public EthernetSwitchPortMapper() {
        super(EthernetSwitchPortResource.class, EthernetSwitchPort.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchPortResource sourcePort, EthernetSwitchPort targetPort) {
        super.performNotAutomatedMapping(source, target);
        ipV4AddressMapper.map(sourcePort.getIpV4Addresses(), targetPort.getIpV4Addresses(), targetPort::addIpV4Address);
        ipV6AddressMapper.map(sourcePort.getIpV6Addresses(), targetPort.getIpV6Addresses(), targetPort::addIpV6Address);
        if (sourcePort.getPriorityFlowControl() != null) {
            PriorityFlowControl targetPriorityFlowControl = mapPriorityFlowControl(
                source.getPriorityFlowControl(),
                ofNullable(target.getPriorityFlowControl()).orElse(new PriorityFlowControl()));
            target.setPriorityFlowControl(targetPriorityFlowControl);
        }
    }

    private PriorityFlowControl mapPriorityFlowControl(PriorityFlowControlObject sourcePriorityFlowControl, PriorityFlowControl targetPriorityFlowControl) {
        targetPriorityFlowControl.setEnabled(sourcePriorityFlowControl.isEnabled());
        simpleTypeMapper.map(
            sourcePriorityFlowControl.getEnabledPriorities(),
            targetPriorityFlowControl.getEnabledPriorities(),
            targetPriorityFlowControl::addEnabledPriority);
        return targetPriorityFlowControl;
    }

}
