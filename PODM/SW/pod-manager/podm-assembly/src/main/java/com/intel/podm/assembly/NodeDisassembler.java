/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.assembly;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ResetActionInvoker;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.common.types.ComposedNodeState.POWERED_ON;
import static java.util.Collections.emptyList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@RequestScoped
public class NodeDisassembler {
    @Inject
    private Logger logger;

    @Inject
    private ResetActionInvoker actionInvoker;

    @Inject
    private VlanTerminator vlanTerminator;

    @Inject
    private VlanSelector vlanSelector;

    @Transactional(value = MANDATORY, rollbackOn = DisassemblyException.class)
    public void disassemble(ComposedNode composedNode) throws DisassemblyException {
        composedNode.getRemoteDrives().forEach(remoteTarget -> remoteTarget.setAllocated(false));

        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem == null) {
            return;
        }

        if (composedNode.isInAnyOfStates(POWERED_ON)) {
            try {
                actionInvoker.shutdownGracefully(computerSystem);
            } catch (ActionException e) {
                throw new DisassemblyException("Could not power off composed node: " + e.getMessage(), e.getErrorResponse(), e);
            }
        }

        cleanUpEthernetInterfaces(computerSystem);
        computerSystem.setAllocated(false);
    }

    private void cleanUpEthernetInterfaces(ComputerSystem computerSystem) {
        for (EthernetInterface ethernetInterface : computerSystem.getEthernetInterfaces()) {
            EthernetSwitchPort neighborSwitchPort = ethernetInterface.getNeighborSwitchPort();
            if (neighborSwitchPort != null) {
                removeVlansFromPort(neighborSwitchPort);
            }
        }
    }

    private void removeVlansFromPort(EthernetSwitchPort ethernetSwitchPort) {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.vlansToDelete(ethernetSwitchPort, emptyList());
        try {
            vlanTerminator.terminate(vlansToDelete);
        } catch (ActionException e) {
            logger.w("Could not remove vlan from port " + ethernetSwitchPort.getSourceUri());
        }
    }
}
