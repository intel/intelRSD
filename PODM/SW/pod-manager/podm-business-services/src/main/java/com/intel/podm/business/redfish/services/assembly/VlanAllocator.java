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

package com.intel.podm.business.redfish.services.assembly;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchPortVlanActionsInvoker;
import com.intel.podm.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import com.intel.podm.client.actions.UpdateVlanRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;
import com.intel.podm.common.types.actions.VlanCreationRequest;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.FAILED;
import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.SUCCESSFUL;
import static com.intel.podm.business.redfish.services.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.podm.common.enterprise.utils.exceptions.RootCauseInvestigator.tryGetExternalServiceErrorInExceptionStack;
import static com.intel.podm.common.types.net.HttpStatusCode.METHOD_NOT_ALLOWED;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
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
    public void createNecessaryVlans(EthernetSwitchPort associatedSwitchPort, List<Vlan> requestedVlans) throws EntityOperationException {
        List<VlanCreationRequest> vlansToCreate = vlanSelector.vlansToCreate(associatedSwitchPort.getEthernetSwitchPortVlans(), requestedVlans);
        for (VlanCreationRequest vlanCreationRequest : vlansToCreate) {
            createVlan(associatedSwitchPort, vlanCreationRequest);
        }
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPortVlan createVlan(EthernetSwitchPort associatedSwitchPort, VlanCreationRequest request) throws EntityOperationException {
        EthernetSwitchPortVlan vlan = ethernetSwitchPortVlanActionsInvoker.create(associatedSwitchPort, request);
        associatedSwitchPort.addEthernetSwitchPortVlan(vlan);
        return vlan;
    }

    @Transactional(MANDATORY)
    public void removeUnnecessaryTaggedVlans(EthernetSwitchPort associatedSwitchPort, List<Vlan> vlansToPreserve) throws EntityOperationException {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.taggedVlansToDelete(associatedSwitchPort, vlansToPreserve);
        vlanTerminator.deleteVlans(vlansToDelete);
    }

    @Transactional(MANDATORY)
    public void changePrimaryVlan(EthernetSwitchPort associatedSwitchPort, Integer primaryVlanId) throws EntityOperationException {
        URI primaryVlanUri = findAssociatedVlanUri(associatedSwitchPort, primaryVlanId);
        EthernetSwitchPortRedefinition switchPortRedefinition = EthernetSwitchPortRedefinition.newBuilder()
            .primaryVlan(primaryVlanUri)
            .build();
        switchPortActionsInvoker.updateSwitchPort(associatedSwitchPort, switchPortRedefinition);
    }

    @Transactional(MANDATORY)
    public VlanRemoveStatus tryRemoveUntaggedVlans(EthernetSwitchPort associatedSwitchPort, Vlan untaggedVlanToPreserve) {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.untaggedVlansToDelete(associatedSwitchPort, untaggedVlanToPreserve);
        try {
            vlanTerminator.deleteVlans(vlansToDelete);
        } catch (EntityOperationException e) {
            if (isResponseMethodNotAllowed(e)) {
                logger.i("Ethernet switch does not allow to remove untagged VLANs. Untagged VLANS will not be removed!");
                return UNSUPPORTED;
            } else {
                logger.e("Cannot remove untagged VLANs, error: {}", e);
                return FAILED;
            }
        }
        return SUCCESSFUL;
    }

    private boolean isResponseMethodNotAllowed(EntityOperationException e) {
        return tryGetExternalServiceErrorInExceptionStack(e)
            .map(error -> Objects.equals(error.getResponse().getHttpStatusCode(), METHOD_NOT_ALLOWED))
            .orElse(false);
    }

    @Transactional(MANDATORY)
    public void updateUntaggedVlan(EthernetSwitchPortVlan vlanToChange, Integer newVlanId) {
        try {
            ethernetSwitchPortVlanActionsInvoker.update(vlanToChange, new UpdateVlanRequest(newVlanId));
        } catch (EntityOperationException e) {
            logger.w("Could not update VLAN: {} with VLAN ID: {}", vlanToChange.getSourceUri(), newVlanId);
        }
    }

    private URI findAssociatedVlanUri(EthernetSwitchPort associatedSwitchPort, Integer primaryVlan) throws EntityOperationException {
        List<EthernetSwitchPortVlan> vlans = associatedSwitchPort.getEthernetSwitchPortVlans().stream()
            .filter(vlan -> vlan.getVlanId().equals(primaryVlan))
            .collect(toList());

        if (vlans.size() != 1) {
            throw new EntityOperationException("There should be exactly one VLAN with vlan id: " + primaryVlan
                + " associated with single switch port, vlans found: " + vlans.size());
        }

        return vlans.iterator().next().getSourceUri();
    }
}

