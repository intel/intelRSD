/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class VlanNodeAssemblyTask extends NodeAssemblyTask {
    private RequestedNode.EthernetInterface requestedInterface;
    private Id availableInterfaceId;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        final String exceptionMessage = "Null value is not allowed for this method.";
        requires(requestedInterface != null, exceptionMessage);
        requires(availableInterfaceId != null, exceptionMessage);

        Optional<List<Vlan>> requestedVlans = requestedInterface.getVlans();
        EthernetInterface ethernetInterface = genericDao.find(EthernetInterface.class, availableInterfaceId);

        Optional<EthernetSwitchPort> associatedSwitchPort = empty();

        try {
            associatedSwitchPort = ofNullable(ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress()));
        } catch (NonUniqueResultException e) {
            logger.e("Could not get Switch Port associated with Ethernet Interface '{}'.", ethernetInterface, e);
        }

        createRequestedVlansOnAssociatedSwitchPort(requestedVlans.orElseThrow(this::illegalStateException),
            associatedSwitchPort.orElseThrow(this::illegalStateException));
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        ComposedNode node = genericDao.find(ComposedNode.class, nodeId);
        return getComputerSystemFromNode(node).getService().getUuid();
    }

    public void setAvailableInterfaceId(Id availableInterfaceId) {
        this.availableInterfaceId = availableInterfaceId;
    }

    public void setRequestedInterface(RequestedNode.EthernetInterface requestedEthernetInterface) {
        this.requestedInterface = requestedEthernetInterface;
    }

    private void createRequestedVlansOnAssociatedSwitchPort(List<RequestedNode.EthernetInterface.Vlan> requestedVlans,
                                                            EthernetSwitchPort associatedSwitchPort) {
        ComposedNode node = genericDao.find(ComposedNode.class, nodeId);

        UUID computerSystemUuid = getComputerSystemFromNode(node).getUuid();
        logger.d("Running for Node: {}, Computer system: {}", node.getId(), computerSystemUuid);
        try {
            vlanAllocator.removeUnnecessaryVlans(associatedSwitchPort, requestedVlans);
            vlanAllocator.createNecessaryVlans(associatedSwitchPort, requestedVlans);

            if (requestedInterface.getPrimaryVlan() != null) {
                vlanAllocator.changePrimaryVlan(associatedSwitchPort, requestedInterface.getPrimaryVlan());
            }
            logger.d("Finished for Node: {}, Computer system: {}", node.getId(), computerSystemUuid);
        } catch (ActionException e) {
            logger.e("Error when creating VLANs for Node: {}, Computer system: {}, switch port: {}, details: {}, API error: {}",
                node.getId(),
                computerSystemUuid,
                associatedSwitchPort.getId(),
                e.getMessage(),
                e.getErrorResponse());
            throw new RuntimeException(e);
        }
    }

    private IllegalStateException illegalStateException() {
        return new IllegalStateException("Expected non-empty option here.");
    }
}
