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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.assembly.VlanAllocator;
import com.intel.podm.assembly.VlanTerminator;
import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.VlanCreationRequest;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VlanNetworkInterfaceActionsService {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private VlanAllocator vlanAllocator;

    @Inject
    private VlanTerminator vlanTerminator;

    @Transactional(REQUIRES_NEW)
    public Context createVlan(Context creationalContext, RedfishVlanNetworkInterface representation) throws BusinessApiException {

        EthernetSwitchPort ethernetSwitchPort = (EthernetSwitchPort) traverser.traverse(creationalContext);

        validateVlanUniqueness(ethernetSwitchPort, representation);

        VlanCreationRequest vlanCreationRequest = buildRequest(representation);
        try {
            EthernetSwitchPortVlan createdVlan = vlanAllocator.createVlan(ethernetSwitchPort, vlanCreationRequest);
            return toContext(createdVlan);
        } catch (ActionException e) {
            throw new EntityOperationException("VLAN creation action could not be completed! " + e.getMessage(), e);
        }
    }

    private void validateVlanUniqueness(EthernetSwitchPort ethernetSwitchPort, RedfishVlanNetworkInterface representation)
        throws ResourceStateMismatchException {

        boolean isVlanUnique = ethernetSwitchPort.getEthernetSwitchPortVlans().stream().noneMatch(vlan ->
            Objects.equals(vlan.getVlanId(), representation.getVlanId()) && Objects.equals(vlan.getTagged(), representation.getTagged()));

        if (!isVlanUnique) {
            throw new ResourceStateMismatchException("VLAN already exists");
        }
    }

    @Transactional(REQUIRES_NEW)
    public void deleteVlan(Context vlanContext) throws ContextResolvingException, EntityOperationException {
        try {
            EthernetSwitchPortVlan ethernetSwitchPortVlan = (EthernetSwitchPortVlan) traverser.traverse(vlanContext);
            vlanTerminator.deleteVlan(ethernetSwitchPortVlan);
        } catch (ActionException e) {
            throw new EntityOperationException("VLAN removal action could not be completed! " + e.getMessage(), e);
        }
    }

    private VlanCreationRequest buildRequest(RedfishVlanNetworkInterface representation) {
        return new VlanCreationRequest(representation.getVlanId(), representation.getTagged(), representation.getVlanEnable());
    }
}
