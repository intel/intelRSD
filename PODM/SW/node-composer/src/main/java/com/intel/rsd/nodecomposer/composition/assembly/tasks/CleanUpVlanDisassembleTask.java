/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.VlanAllocator;
import com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus;
import com.intel.rsd.nodecomposer.composition.assembly.VlanSelector;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;

import static com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.Collections.emptyList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class CleanUpVlanDisassembleTask extends NodeRemovalTask implements Serializable {
    private static final long serialVersionUID = -6136031740361772861L;

    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient VlanAllocator vlanAllocator;
    @Autowired
    private transient VlanSelector vlanSelector;

    private ODataId ethernetSwitchPortODataId;

    @Override
    public void deallocate() {
    }

    @Override
    public void disassemble() {
        requiresNonNull(composedNodeODataId, "composedNodeODataId");
        ComposedNode composedNode = genericDao.find(ComposedNode.class, composedNodeODataId);
        requiresNonNull(composedNode, "composedNode");
        requiresNonNull(ethernetSwitchPortODataId, "ethernetSwitchPortODataId");
        EthernetSwitchPort ethernetSwitchPort = genericDao.find(EthernetSwitchPort.class, ethernetSwitchPortODataId);
        removeVlansFromPort(ethernetSwitchPort, composedNode.getPriorUntaggedVlanId());
    }

    private void removeVlansFromPort(EthernetSwitchPort ethernetSwitchPort, Integer untaggedVlanIdBeforeAllocation) {
        try {
            vlanAllocator.removeUnnecessaryTaggedVlans(ethernetSwitchPort, emptyList());
        } catch (EntityOperationException e) {
            log.warn("Could not remove VLAN from port " + ethernetSwitchPort.getUri());
        }

        VlanRemoveStatus vlanRemoveStatus = vlanAllocator.tryRemoveUntaggedVlans(ethernetSwitchPort, null);
        if (vlanRemoveStatus.equals(UNSUPPORTED) && untaggedVlanIdBeforeAllocation != null) {
            vlanSelector.tryGetUntaggedVlanToChange(ethernetSwitchPort)
                .ifPresent(vlanToChange -> vlanAllocator.updateUntaggedVlan(vlanToChange, untaggedVlanIdBeforeAllocation));
        }
    }

    public NodeRemovalTask init(ODataId ethernetSwitchPortODataId) {
        this.ethernetSwitchPortODataId = ethernetSwitchPortODataId;
        return this;
    }
}
