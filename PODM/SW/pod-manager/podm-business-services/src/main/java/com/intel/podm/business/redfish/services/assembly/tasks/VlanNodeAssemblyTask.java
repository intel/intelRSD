/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.services.assembly.VlanAllocator;
import com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus;
import com.intel.podm.business.redfish.services.assembly.VlanSelector;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.podm.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.singletonList;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class VlanNodeAssemblyTask extends NodeTask {

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

    @Inject
    private VlanSelector vlanSelector;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        validate();
        List<Vlan> requestedVlans = retrieveVlans();
        EthernetSwitchPort associatedSwitchPort = retrieveEthernetSwitchPort();
        createRequestedVlansOnAssociatedSwitchPort(requestedVlans, associatedSwitchPort);
    }

    private void validate() {
        requiresNonNull(availableInterfaceId, "Id of Node's EthernetInterface cannot be null");
        requiresNonNull(requestedInterface, "RequestedInterface definition cannot be null");
    }

    private EthernetSwitchPort retrieveEthernetSwitchPort() {
        EthernetInterface ethernetInterface = genericDao.find(EthernetInterface.class, availableInterfaceId);
        String errorMessage = format("Could not get Switch Port associated with Ethernet Interface '%s'.", ethernetInterface);
        try {
            return ofNullable(ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress()))
                .orElseThrow(() -> new IllegalStateException(errorMessage));
        } catch (NonUniqueResultException e) {
            logger.e(errorMessage, e);
            throw new IllegalStateException(errorMessage, e);
        }
    }

    private List<Vlan> retrieveVlans() {
        if (!requestedInterface.getVlans().isPresent() && requestedInterface.getPrimaryVlan() != null) {
            return singletonList(getVlanFromPrimaryVlan());
        }
        return requestedInterface.getVlans().orElseThrow(() -> new IllegalStateException("Vlans or PrimaryVlan is required in RequestedInterface"));
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        EthernetSwitchPort ethernetSwitchPort = retrieveEthernetSwitchPort();
        return ethernetSwitchPort.getService().getUuid();
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
            vlanAllocator.removeUnnecessaryTaggedVlans(associatedSwitchPort, requestedVlans);
            prepareUntaggedVlan(associatedSwitchPort, getRequestedUntaggedVlan(requestedVlans), node);
            vlanAllocator.createNecessaryVlans(associatedSwitchPort, requestedVlans);
            changePrimaryVlan(associatedSwitchPort);

            logger.d("Finished for Node: {}, Computer system: {}", node.getId(), computerSystemUuid);
        } catch (EntityOperationException e) {
            logger.e(format("Error when creating VLANs for Node: %s, Computer system: %s, switch port: %s, details: %s",
                node.getId(),
                computerSystemUuid,
                associatedSwitchPort.getId(),
                e.getMessage()), e);
            throw new RuntimeException(e);
        }
    }

    private Vlan getRequestedUntaggedVlan(List<Vlan> requestedVlans) {
        return requestedVlans.stream()
            .filter(vlan -> !vlan.isTagged())
            .findFirst()
            .orElse(null);
    }

    private void changePrimaryVlan(EthernetSwitchPort associatedSwitchPort) throws EntityOperationException {
        if (primaryVlanShouldBeChanged(associatedSwitchPort)) {
            vlanAllocator.changePrimaryVlan(associatedSwitchPort, requestedInterface.getPrimaryVlan());
        }
    }

    private boolean primaryVlanShouldBeChanged(EthernetSwitchPort associatedSwitchPort) {
        return requestedInterface.getPrimaryVlan() != null && !isPrimaryVlanSetOnEthernetSwitchPort(associatedSwitchPort);
    }

    private boolean isPrimaryVlanSetOnEthernetSwitchPort(EthernetSwitchPort associatedSwitchPort) {
        EthernetSwitchPortVlan primaryVlan = associatedSwitchPort.getPrimaryVlan();
        return primaryVlan != null && Objects.equals(primaryVlan.getVlanId(), requestedInterface.getPrimaryVlan());
    }

    private void prepareUntaggedVlan(EthernetSwitchPort associatedSwitchPort, Vlan requestedUntaggedVlan, ComposedNode node) throws EntityOperationException {
        VlanRemoveStatus vlanRemoveStatus = vlanAllocator.tryRemoveUntaggedVlans(associatedSwitchPort, requestedUntaggedVlan);
        if (requestedUntaggedVlan != null && Objects.equals(vlanRemoveStatus, UNSUPPORTED)) {
            vlanSelector.tryGetUntaggedVlanToChange(associatedSwitchPort).ifPresent(vlanToChange -> {
                node.setPriorUntaggedVlanId(vlanToChange.getVlanId());
                vlanAllocator.updateUntaggedVlan(vlanToChange, requestedUntaggedVlan.getVlanId());
            });
        }
    }

    @SuppressWarnings("checkstyle:AnonInnerLength")
    private Vlan getVlanFromPrimaryVlan() {
        return new Vlan() {
            @Override
            public boolean isTagged() {
                return false;
            }

            @Override
            public Integer getVlanId() {
                return requestedInterface.getPrimaryVlan();
            }

            @Override
            public Boolean isEnabled() {
                return null;
            }
        };
    }
}
