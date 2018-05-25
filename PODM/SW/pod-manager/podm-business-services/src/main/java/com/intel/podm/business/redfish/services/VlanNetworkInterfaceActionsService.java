/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.EthernetSwitchPortVlanActionsInvoker;
import com.intel.podm.business.redfish.services.assembly.VlanAllocator;
import com.intel.podm.business.redfish.services.assembly.VlanTerminator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.actions.UpdateVlanRequest;
import com.intel.podm.common.types.actions.VlanCreationRequest;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.function.Predicate;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VlanNetworkInterfaceActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private VlanTerminator vlanTerminator;

    @Inject
    private EthernetSwitchPortVlanActionsInvoker invoker;

    @Transactional(REQUIRES_NEW)
    public Context createVlan(Context creationalContext, RedfishVlanNetworkInterface representation) throws BusinessApiException {
        EthernetSwitchPort ethernetSwitchPort = (EthernetSwitchPort) traverser.traverse(creationalContext);
        validateVlanUniqueness(ethernetSwitchPort, representation);

        EthernetSwitchPortVlan createdVlan = vlanAllocator.createVlan(ethernetSwitchPort, buildRequest(representation));
        return toContext(createdVlan);
    }

    @Transactional(REQUIRES_NEW)
    public void updateVlan(Context context, RedfishVlanNetworkInterface representation) throws BusinessApiException {
        EthernetSwitchPortVlan vlan = (EthernetSwitchPortVlan) traverser.traverse(context);
        validateVlanIdsUniqueness(vlan, representation.getVlanId());

        invoker.update(vlan, new UpdateVlanRequest(representation.getVlanId()));
    }

    @Transactional(REQUIRES_NEW)
    public void deleteVlan(Context vlanContext) throws ContextResolvingException, EntityOperationException {
        EthernetSwitchPortVlan ethernetSwitchPortVlan = (EthernetSwitchPortVlan) traverser.traverse(vlanContext);
        vlanTerminator.deleteVlan(ethernetSwitchPortVlan);
    }

    private void validateVlanUniqueness(EthernetSwitchPort ethernetSwitchPort, RedfishVlanNetworkInterface representation)
        throws ResourceStateMismatchException {

        boolean isVlanUnique = ethernetSwitchPort.getEthernetSwitchPortVlans().stream()
            .noneMatch(vlanWithSameVlanIdAndTaggedStateAlreadyExistsPredicate(representation.getVlanId(), representation.getTagged()));

        if (!isVlanUnique) {
            throw new ResourceStateMismatchException("VLAN already exists");
        }
    }

    private void validateVlanIdsUniqueness(EthernetSwitchPortVlan vlan, Integer targetVlanId)
        throws ResourceStateMismatchException {

        boolean isVlanUnique = vlan.getEthernetSwitchPort()
            .getEthernetSwitchPortVlans().stream()
            .filter(excludeSourceVlanPredicate(vlan.getVlanId()))
            .noneMatch(vlanWithSameVlanIdAndTaggedStateAlreadyExistsPredicate(targetVlanId, vlan.getTagged()));

        if (!isVlanUnique) {
            throw new ResourceStateMismatchException("VLAN with that VLANId already exists");
        }
    }

    private Predicate<EthernetSwitchPortVlan> excludeSourceVlanPredicate(Integer sourceVlanId) {
        return vlan -> !Objects.equals(vlan.getVlanId(), sourceVlanId);
    }

    private Predicate<EthernetSwitchPortVlan> vlanWithSameVlanIdAndTaggedStateAlreadyExistsPredicate(Integer vlanId, Boolean tagged) {
        return vlan -> Objects.equals(vlan.getVlanId(), vlanId) && Objects.equals(vlan.getTagged(), tagged);
    }

    private VlanCreationRequest buildRequest(RedfishVlanNetworkInterface representation) {
        return new VlanCreationRequest(representation.getVlanId(), representation.getTagged(), representation.getVlanEnable());
    }
}
