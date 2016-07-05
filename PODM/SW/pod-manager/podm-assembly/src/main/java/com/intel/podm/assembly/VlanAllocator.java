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
import com.intel.podm.actions.EthernetSwitchPortActionsInvoker;
import com.intel.podm.actions.EthernetSwitchPortVlanActionsInvoker;
import com.intel.podm.actions.EthernetSwitchPortVlanActionsInvoker.VlanCreationRequest;
import com.intel.podm.actions.EthernetSwitchPortVlanObtainer;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.EthernetSwitchPortRedefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static java.util.Optional.empty;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
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
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private EthernetSwitchPortVlanObtainer vlanObtainer;

    @Inject
    private Logger logger;

    @Transactional(REQUIRED)
    public void createNecessaryVlans(Id associatedSwitchPortId, List<RequestedEthernetInterface.Vlan> requestedVlans) throws ActionException {
        EthernetSwitchPort associatedSwitchPort = ethernetSwitchPortDao.getOrThrow(associatedSwitchPortId);
        List<VlanCreationRequest> vlansToCreate = vlanSelector.vlansToCreate(associatedSwitchPort.getVlans(), requestedVlans);
        for (VlanCreationRequest vlanCreationRequest : vlansToCreate) {
            URI vlanUri = ethernetSwitchPortVlanActionsInvoker.create(associatedSwitchPort.getId(), vlanCreationRequest);
            try {
                vlanObtainer.discoverEthernetSwitchPortVlan(associatedSwitchPort.getId(), vlanUri);
            } catch (ExternalServiceApiReaderException e) {
                String errorMessage = "Vlan creation was successful, but failed on refreshing selected Vlan";
                logger.i(errorMessage + " on [ service: {}, path: {} ]", associatedSwitchPort.getService().getBaseUri(), e.getResourceUri());
                throw new ActionException(errorMessage, e.getErrorResponse());
            }
        }
    }

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void removeUnnecessaryVlans(Id associatedSwitchPortId, List<RequestedEthernetInterface.Vlan> requestedVlans)
            throws ActionException {
        EthernetSwitchPort associatedSwitchPort = ethernetSwitchPortDao.getOrThrow(associatedSwitchPortId);
        List<EthernetSwitchPortVlan> vlansToDelete = vlanSelector.vlansToDelete(associatedSwitchPort, requestedVlans);
        vlanTerminator.terminate(vlansToDelete);
    }

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void changePrimaryVlan(Id associatedSwitchPortId, Integer primaryVlanId) throws ActionException {
        EthernetSwitchPort associatedSwitchPort = ethernetSwitchPortDao.getOrThrow(associatedSwitchPortId);
        URI primaryVlanUri = findAssociatedVlanUri(associatedSwitchPort, primaryVlanId);
        EthernetSwitchPortRedefinition switchPortRedefinition = EthernetSwitchPortRedefinition.newBuilder()
                .primaryVlan(primaryVlanUri)
                .uris(empty())
                .build();
        switchPortActionsInvoker.updateSwitchPort(associatedSwitchPort, switchPortRedefinition);
    }

    private URI findAssociatedVlanUri(EthernetSwitchPort associatedSwitchPort, Integer primaryVlan) throws ActionException {
        List<EthernetSwitchPortVlan> vlans = associatedSwitchPort.getVlans().stream()
                .filter(vlan -> vlan.getVlanId().equals(primaryVlan))
                .collect(toList());

        if (vlans.size() != 1) {
            throw new ActionException("There should be exactly one VLAN with vlan id: " + primaryVlan
                    + " associated with single switch port, vlans found: " + vlans.size());
        }

        return vlans.iterator().next().getSourceUri();
    }
}

