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

package com.intel.podm.assembly.tasks;

import com.intel.podm.actions.ActionException;
import com.intel.podm.assembly.VlanAllocator;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static com.google.common.base.Preconditions.checkArgument;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class VlanNodeAssemblyTask extends NodeAssemblyTask {
    private RequestedEthernetInterface requestedInterface;
    private Id availableInterfaceId;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() {
        final String exceptionMessage = "Null value is not allowed for this method.";
        checkArgument(requestedInterface != null, exceptionMessage);
        checkArgument(availableInterfaceId != null, exceptionMessage);

        Optional<List<RequestedEthernetInterface.Vlan>> requestedVlans = requestedInterface.getVlans();
        EthernetInterface ethernetInterface = ethernetInterfaceDao.find(availableInterfaceId);

        Optional<EthernetSwitchPort> associatedSwitchPort = ofNullable(ethernetInterface.getNeighborSwitchPort());

        createRequestedVlansOnAssociatedSwitchPort(requestedVlans.orElseThrow(this::illegalStateException),
                associatedSwitchPort.orElseThrow(this::illegalStateException));
    }

    public void setAvailableInterfaceId(Id availableInterfaceId) {
        this.availableInterfaceId = availableInterfaceId;
    }

    public void setRequestedInterface(RequestedEthernetInterface requestedEthernetInterface) {
        this.requestedInterface = requestedEthernetInterface;
    }

    private void createRequestedVlansOnAssociatedSwitchPort(List<RequestedEthernetInterface.Vlan> requestedVlans, EthernetSwitchPort associatedSwitchPort) {
        ComposedNode node = genericDao.find(ComposedNode.class, nodeId);

        UUID computerSystemUuid = getComputerSystemFromNode(node).getUuid();
        logger.d("Running for Node: {}, Computer system: {}", node.getId(), computerSystemUuid);
        Id associatedSwitchPortId = associatedSwitchPort.getId();
        try {
            vlanAllocator.removeUnnecessaryVlans(associatedSwitchPortId, requestedVlans);
            vlanAllocator.createNecessaryVlans(associatedSwitchPortId, requestedVlans);

            if (requestedInterface.getPrimaryVlan() != null) {
                vlanAllocator.changePrimaryVlan(associatedSwitchPortId, requestedInterface.getPrimaryVlan());
            }
            logger.d("Finished for Node: {}, Computer system: {}", node.getId(), computerSystemUuid);
        } catch (ActionException e) {
            logger.e("Error when creating VLANs for Node: {}, Computer system: {}, switch port: {}, details: {}, API error: {}",
                    node.getId(),
                    computerSystemUuid,
                    associatedSwitchPortId,
                    e.getMessage(),
                    e.getErrorResponse());
            throw new RuntimeException(e);
        }
    }

    private IllegalStateException illegalStateException() {
        return new IllegalStateException("Expected non-empty option here.");
    }
}
