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

package com.intel.podm.assembly.tasks;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ResetActionInvoker;
import com.intel.podm.assembly.NodeDisassembler;
import com.intel.podm.assembly.VlanSelector;
import com.intel.podm.assembly.VlanTerminator;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ComposedNodeState;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.UUID;

import static java.util.Collections.emptyList;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemDisassembleTask extends NodeAssemblyTask {
    @Inject
    private NodeDisassembler nodeDisassembler;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private ResetActionInvoker actionInvoker;

    @Inject
    private VlanTerminator vlanTerminator;

    @Inject
    private VlanSelector vlanSelector;

    @Inject
    private Logger logger;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        disassemble(composedNode);
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return getComputerSystemFromNode(genericDao.find(ComposedNode.class, nodeId)).getService().getUuid();
    }

    private void disassemble(ComposedNode composedNode) {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem == null) {
            return;
        }

        if (composedNode.isInAnyOfStates(ComposedNodeState.ASSEMBLED)) {
            try {
                actionInvoker.powerOff(computerSystem);
            } catch (ActionException e) {
                logger.e("Could not power off composed node", e);
                throw new RuntimeException("Could not power off composed node", e);
            }
        }

        cleanUpEthernetInterfaces(computerSystem);
        computerSystem.getMetadata().setAllocated(false);
    }


    private void cleanUpEthernetInterfaces(ComputerSystem computerSystem) {
        for (EthernetInterface ethernetInterface : computerSystem.getEthernetInterfaces()) {
            try {
                EthernetSwitchPort neighborSwitchPort =
                        ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress());
                if (neighborSwitchPort != null) {
                    removeVlansFromPort(neighborSwitchPort);
                }
            } catch (NonUniqueResultException e) {
                logger.e("Could not clean up Ethernet Interface '{}'.", ethernetInterface, e);
            }

        }
    }

    private void removeVlansFromPort(EthernetSwitchPort ethernetSwitchPort) {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.vlansToDelete(ethernetSwitchPort, emptyList());
        try {
            vlanTerminator.deleteVlans(vlansToDelete);
        } catch (ActionException e) {
            logger.w("Could not remove vlan from port " + ethernetSwitchPort.getSourceUri());
        }
    }
}
