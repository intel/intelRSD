/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.services.actions.ChangeTpmStateActionsInvoker;
import com.intel.podm.business.redfish.services.actions.ComputerSystemUpdateInvoker;
import com.intel.podm.business.redfish.services.actions.ResetActionInvoker;
import com.intel.podm.business.redfish.services.assembly.VlanAllocator;
import com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus;
import com.intel.podm.business.redfish.services.assembly.VlanSelector;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemDisassembleTask extends NodeRemovalTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private ResetActionInvoker actionInvoker;

    @Inject
    private ComputerSystemUpdateInvoker computerSystemUpdateInvoker;

    @Inject
    private ChangeTpmStateActionsInvoker changeTpmStateActionsInvoker;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private VlanSelector vlanSelector;

    @Inject
    private Logger logger;

    private ComposedNode composedNode;
    private ComputerSystem computerSystem;


    @Override
    public void deallocate() {
        setComposedNodeAndComputerSystem();
        if (computerSystem == null) {
            logger.e("Couldn't deallocate computerSystem for nodeId={}, computerSystem is null", nodeId);
            return;
        }
        computerSystem.getMetadata().setAllocated(false);
    }

    @Override
    public void disassemble() {
        setComposedNodeAndComputerSystem();
        if (composedNode == null || computerSystem == null) {
            logger.e("Couldn't disassemble computerSystem for nodeId={}, computerSystem or composedNode is null", nodeId);
            return;
        }
        enableFirmwareUpdateOnComputerSystem();
        clearTpmOwnership();
        powerOffAssembledComposedNode();
        cleanUpVlansOnNeighborSwitchPorts();
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return getAssociatedComputeServiceUuid(genericDao.find(ComposedNode.class, nodeId));
    }

    private void setComposedNodeAndComputerSystem() {
        composedNode = ofNullable(composedNode).orElse(genericDao.find(ComposedNode.class, nodeId));
        if (composedNode != null) {
            computerSystem = ofNullable(computerSystem).orElse(composedNode.getComputerSystem());
        }
    }

    private void powerOffAssembledComposedNode() {
        if (composedNode.isInAnyOfStates(ASSEMBLED)) {
            try {
                actionInvoker.powerOff(computerSystem);
            } catch (EntityOperationException e) {
                logger.e("Could not power off composed node", e);
                throw new RuntimeException("Could not power off composed node", e);
            }
        }
    }

    private void clearTpmOwnership() {
        if (nullOrEmpty(computerSystem.getTrustedModules())) {
            return;
        }

        try {
            ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition = new ChangeTpmStatusUpdateDefinition(TRUE);
            changeTpmStateActionsInvoker.changeTpmState(computerSystem, changeTpmStatusUpdateDefinition);
        } catch (EntityOperationException e) {
            throw new RuntimeException("Clear ownership of TPM failed on Computer system", e);
        }
    }

    private void enableFirmwareUpdateOnComputerSystem() {
        if (computerSystem.getUserModeEnabled() != null) {
            ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();
            computerSystemUpdateDefinition.setUserModeEnabled(FALSE);
            try {
                computerSystemUpdateInvoker.updateComputerSystem(computerSystem, computerSystemUpdateDefinition);
            } catch (EntityOperationException e) {
                throw new RuntimeException("Enabling firmware update failed on computer system", e);
            }
        }
    }

    private Set<EthernetSwitchPort> getNeighborSwitchPorts(ComputerSystem computerSystem) {
        Set<EthernetSwitchPort> neighborEthernetSwitchPorts = new HashSet<>();
        for (EthernetInterface ethernetInterface : computerSystem.getEthernetInterfaces()) {
            try {
                EthernetSwitchPort neighborSwitchPort =
                    ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress());
                if (neighborSwitchPort != null) {
                    neighborEthernetSwitchPorts.add(neighborSwitchPort);
                }
            } catch (NonUniqueResultException e) {
                logger.e(format("Could not get neighbor switch port for Ethernet Interface '%s'.", ethernetInterface), e);
            }
        }
        return neighborEthernetSwitchPorts;
    }

    private void cleanUpVlansOnNeighborSwitchPorts() {
        Set<EthernetSwitchPort> neighborEthernetSwitchPorts = getNeighborSwitchPorts(computerSystem);
        Integer untaggedVlanIdBeforeAllocation = composedNode.getPriorUntaggedVlanId();
        neighborEthernetSwitchPorts.forEach(ethernetSwitchPort -> removeVlansFromPort(ethernetSwitchPort, untaggedVlanIdBeforeAllocation));
    }

    private void removeVlansFromPort(EthernetSwitchPort ethernetSwitchPort, Integer untaggedVlanIdBeforeAllocation) {
        try {
            vlanAllocator.removeUnnecessaryTaggedVlans(ethernetSwitchPort, emptyList());
        } catch (EntityOperationException e) {
            logger.w("Could not remove VLAN from port " + ethernetSwitchPort.getSourceUri());
        }

        VlanRemoveStatus vlanRemoveStatus = vlanAllocator.tryRemoveUntaggedVlans(ethernetSwitchPort, null);
        if (vlanRemoveStatus.equals(UNSUPPORTED) && untaggedVlanIdBeforeAllocation != null) {
            vlanSelector.untaggedVlanToChange(ethernetSwitchPort)
                .ifPresent(vlanToChange -> vlanAllocator.updateUntaggedVlan(vlanToChange, untaggedVlanIdBeforeAllocation));
        }
    }
}
