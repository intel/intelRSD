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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.services.assembly.VlanAllocator;
import com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus;
import com.intel.podm.business.redfish.services.assembly.VlanSelector;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Collections.emptyList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class CleanUpVlanDisassembleTask extends NodeRemovalTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private VlanSelector vlanSelector;

    @Inject
    private Logger logger;

    private Id ethernetSwitchPortId;


    @Override
    public void deallocate() {
    }

    @Override
    public void disassemble() {
        requiresNonNull(nodeId, "nodeId");
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        requiresNonNull(composedNode, "composedNode");
        requiresNonNull(ethernetSwitchPortId, "ethernetSwitchPortId");
        EthernetSwitchPort ethernetSwitchPort = genericDao.find(EthernetSwitchPort.class, ethernetSwitchPortId);
        removeVlansFromPort(ethernetSwitchPort, composedNode.getPriorUntaggedVlanId());
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        requiresNonNull(ethernetSwitchPortId, "ethernetSwitchPortId");
        return genericDao.find(EthernetSwitchPort.class, ethernetSwitchPortId).getService().getUuid();
    }

    private void removeVlansFromPort(EthernetSwitchPort ethernetSwitchPort, Integer untaggedVlanIdBeforeAllocation) {
        try {
            vlanAllocator.removeUnnecessaryTaggedVlans(ethernetSwitchPort, emptyList());
        } catch (EntityOperationException e) {
            logger.w("Could not remove VLAN from port " + ethernetSwitchPort.getSourceUri());
        }

        VlanRemoveStatus vlanRemoveStatus = vlanAllocator.tryRemoveUntaggedVlans(ethernetSwitchPort, null);
        if (vlanRemoveStatus.equals(UNSUPPORTED) && untaggedVlanIdBeforeAllocation != null) {
            vlanSelector.tryGetUntaggedVlanToChange(ethernetSwitchPort)
                .ifPresent(vlanToChange -> vlanAllocator.updateUntaggedVlan(vlanToChange, untaggedVlanIdBeforeAllocation));
        }
    }

    public NodeRemovalTask init(Id ethernetSwitchPortId) {
        this.ethernetSwitchPortId = ethernetSwitchPortId;
        return this;
    }
}
