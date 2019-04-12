/*
 * Copyright (c) 2015-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.assembly.VlanAllocator;
import com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus;
import com.intel.rsd.nodecomposer.composition.assembly.VlanSelector;
import com.intel.rsd.nodecomposer.persistence.NonUniqueResultException;
import com.intel.rsd.nodecomposer.persistence.dao.EthernetSwitchPortDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import static com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.singletonList;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VlanNodeAssemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 4053486827370818055L;

    @Autowired
    private transient VlanAllocator vlanAllocator;
    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient EthernetSwitchPortDao ethernetSwitchPortDao;
    @Autowired
    private transient VlanSelector vlanSelector;

    @Setter
    private RequestedNode.EthernetInterface requestedInterface;
    @Setter
    private ODataId availableInterfaceODataId;

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
        requiresNonNull(availableInterfaceODataId, "Id of Node's EthernetInterface cannot be null");
        requiresNonNull(requestedInterface, "RequestedInterface definition cannot be null");
    }

    private EthernetSwitchPort retrieveEthernetSwitchPort() {
        EthernetInterface ethernetInterface = genericDao.find(EthernetInterface.class, availableInterfaceODataId);
        String errorMessage = format("Could not get Switch Port associated with Ethernet Interface '%s'.", ethernetInterface);
        try {
            return ofNullable(ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress()))
                .orElseThrow(() -> new IllegalStateException(errorMessage));
        } catch (NonUniqueResultException e) {
            log.error(errorMessage, e);
            throw new IllegalStateException(errorMessage, e);
        }
    }

    private List<Vlan> retrieveVlans() {
        if (!requestedInterface.getVlans().isPresent() && requestedInterface.getPrimaryVlan() != null) {
            return singletonList(getVlanFromPrimaryVlan());
        }
        return requestedInterface.getVlans().orElseThrow(() -> new IllegalStateException("Vlans or PrimaryVlan is required in RequestedInterface"));
    }

    private void createRequestedVlansOnAssociatedSwitchPort(List<RequestedNode.EthernetInterface.Vlan> requestedVlans,
                                                            EthernetSwitchPort associatedSwitchPort) {
        ComposedNode node = genericDao.find(ComposedNode.class, composedNodeODataId);
        UUID computerSystemUuid = getComputerSystemFromNode(node).getUuid();
        log.debug("Running for Node: {}, Computer system: {}", node.getUri(), computerSystemUuid);
        try {
            vlanAllocator.removeUnnecessaryTaggedVlans(associatedSwitchPort, requestedVlans);
            prepareUntaggedVlan(associatedSwitchPort, getRequestedUntaggedVlan(requestedVlans), node);
            vlanAllocator.createNecessaryVlans(associatedSwitchPort, requestedVlans);
            changePrimaryVlan(associatedSwitchPort);

            log.debug("Finished for Node: {}, Computer system: {}", node.getUri(), computerSystemUuid);
        } catch (EntityOperationException e) {
            log.error(format("Error when creating VLANs for Node: %s, Computer system: %s, switch port: %s, details: %s",
                node.getUri(),
                computerSystemUuid,
                associatedSwitchPort.getUri(),
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
