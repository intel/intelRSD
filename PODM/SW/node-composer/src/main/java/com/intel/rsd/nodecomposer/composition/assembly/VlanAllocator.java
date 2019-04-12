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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.EthernetSwitchPortActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.EthernetSwitchPortVlanActionsInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import com.intel.rsd.nodecomposer.externalservices.actions.UpdateVlanRequest;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import com.intel.rsd.nodecomposer.types.actions.EthernetSwitchPortRedefinition;
import com.intel.rsd.nodecomposer.types.actions.VlanCreationRequest;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus.FAILED;
import static com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus.SUCCESSFUL;
import static com.intel.rsd.nodecomposer.composition.assembly.VlanRemoveStatus.UNSUPPORTED;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.METHOD_NOT_ALLOWED;
import static com.intel.rsd.nodecomposer.utils.exceptions.RootCauseInvestigator.tryGetExternalServiceErrorInExceptionStack;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VlanAllocator {
    private final VlanSelector vlanSelector;
    private final VlanTerminator vlanTerminator;
    private final EthernetSwitchPortVlanActionsInvoker ethernetSwitchPortVlanActionsInvoker;
    private final EthernetSwitchPortActionsInvoker switchPortActionsInvoker;

    @Autowired
    public VlanAllocator(VlanSelector vlanSelector, VlanTerminator vlanTerminator, EthernetSwitchPortVlanActionsInvoker ethernetSwitchPortVlanActionsInvoker,
                         EthernetSwitchPortActionsInvoker switchPortActionsInvoker) {
        this.vlanSelector = vlanSelector;
        this.vlanTerminator = vlanTerminator;
        this.ethernetSwitchPortVlanActionsInvoker = ethernetSwitchPortVlanActionsInvoker;
        this.switchPortActionsInvoker = switchPortActionsInvoker;
    }

    @Transactional(MANDATORY)
    public void createNecessaryVlans(EthernetSwitchPort associatedSwitchPort, List<Vlan> requestedVlans) throws EntityOperationException {
        List<VlanCreationRequest> vlansToCreate = vlanSelector.getVlansToCreate(associatedSwitchPort.getEthernetSwitchPortVlans(), requestedVlans);
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
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.getTaggedVlansToDelete(associatedSwitchPort, vlansToPreserve);
        vlanTerminator.deleteVlans(vlansToDelete);
    }

    @Transactional(MANDATORY)
    public void changePrimaryVlan(EthernetSwitchPort associatedSwitchPort, Integer primaryVlanId) throws EntityOperationException {
        ODataId primaryVlanUri = findAssociatedVlanUri(associatedSwitchPort, primaryVlanId);
        EthernetSwitchPortRedefinition switchPortRedefinition = EthernetSwitchPortRedefinition.newBuilder()
            .primaryVlan(primaryVlanUri)
            .build();
        switchPortActionsInvoker.updateSwitchPort(associatedSwitchPort, switchPortRedefinition);
    }

    @Transactional(MANDATORY)
    public VlanRemoveStatus tryRemoveUntaggedVlans(EthernetSwitchPort associatedSwitchPort, Vlan untaggedVlanToPreserve) {
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.getUntaggedVlansToDelete(associatedSwitchPort, untaggedVlanToPreserve);
        try {
            vlanTerminator.deleteVlans(vlansToDelete);
        } catch (EntityOperationException e) {
            if (isResponseMethodNotAllowed(e)) {
                log.info("Ethernet switch does not allow to remove untagged VLANs. Untagged VLANS will not be removed!");
                return UNSUPPORTED;
            } else {
                log.error("Cannot remove untagged VLANs, error: {}", e);
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
            log.warn("Could not update VLAN: {} with VLAN ID: {}", vlanToChange.getUri(), newVlanId);
        }
    }

    private ODataId findAssociatedVlanUri(EthernetSwitchPort associatedSwitchPort, Integer primaryVlan) throws EntityOperationException {
        List<EthernetSwitchPortVlan> vlans = associatedSwitchPort.getEthernetSwitchPortVlans().stream()
            .filter(vlan -> vlan.getVlanId().equals(primaryVlan))
            .collect(toList());

        if (vlans.size() != 1) {
            throw new EntityOperationException("There should be exactly one VLAN with vlan id: " + primaryVlan
                + " associated with single switch port, vlans found: " + vlans.size());
        }

        return vlans.iterator().next().getUri();
    }
}
