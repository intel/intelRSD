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

package com.intel.podm.assembly;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.actions.EthernetSwitchPortVlanActionsInvoker;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;
import com.intel.podm.common.types.actions.VlanCreationRequest;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class VlanAllocator {
    @Inject
    private VlanSelector vlanSelector;

    @Inject
    private VlanTerminator vlanTerminator;

    @Inject
    private EthernetSwitchPortVlanActionsInvoker ethernetSwitchPortVlanActionsInvoker;

    @Inject
    private EthernetSwitchPortActionsInvoker switchPortActionsInvoker;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void createNecessaryVlans(EthernetSwitchPort associatedSwitchPort, List<RequestedNode.EthernetInterface.Vlan> requestedVlans)
            throws ActionException {
        List<VlanCreationRequest> vlansToCreate = vlanSelector.vlansToCreate(associatedSwitchPort.getEthernetSwitchPortVlans(), requestedVlans);
        for (VlanCreationRequest vlanCreationRequest : vlansToCreate) {
            createVlan(associatedSwitchPort, vlanCreationRequest);
        }
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan createVlan(EthernetSwitchPort associatedSwitchPort, VlanCreationRequest vlanCreationRequest) throws ActionException {
        EthernetSwitchPortVlan vlan = ethernetSwitchPortVlanActionsInvoker.create(associatedSwitchPort, vlanCreationRequest);
        associatedSwitchPort.addEthernetSwitchPortVlan(vlan);
        return vlan;
    }

    @Transactional(MANDATORY)
    public void removeUnnecessaryVlans(EthernetSwitchPort associatedSwitchPort, List<RequestedNode.EthernetInterface.Vlan> requestedVlans)
            throws ActionException {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.vlansToDelete(associatedSwitchPort, requestedVlans);
        vlanTerminator.deleteVlans(vlansToDelete);
    }

    @Transactional(MANDATORY)
    public void changePrimaryVlan(EthernetSwitchPort associatedSwitchPort, Integer primaryVlanId) throws ActionException {
        URI primaryVlanUri = findAssociatedVlanUri(associatedSwitchPort, primaryVlanId);
        EthernetSwitchPortRedefinition switchPortRedefinition = EthernetSwitchPortRedefinition.newBuilder()
                .primaryVlan(primaryVlanUri)
                .build();
        switchPortActionsInvoker.updateSwitchPort(associatedSwitchPort, switchPortRedefinition);
    }

    private URI findAssociatedVlanUri(EthernetSwitchPort associatedSwitchPort, Integer primaryVlan) throws ActionException {
        List<EthernetSwitchPortVlan> vlans = associatedSwitchPort.getEthernetSwitchPortVlans().stream()
                .filter(vlan -> vlan.getVlanId().equals(primaryVlan))
                .collect(toList());

        if (vlans.size() != 1) {
            throw new ActionException("There should be exactly one VLAN with vlan id: " + primaryVlan
                    + " associated with single switch port, vlans found: " + vlans.size());
        }

        return vlans.iterator().next().getSourceUri();
    }
}

