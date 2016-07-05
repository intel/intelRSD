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

package com.intel.podm.actions;

import com.intel.podm.business.entities.dao.EthernetSwitchDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActions;
import com.intel.podm.client.api.actions.EthernetSwitchPortResourceActionsFactory;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortResource;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.EthernetSwitchPortMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.intel.podm.client.api.reader.ResourceSupplier.getUrisFromResources;
import static com.intel.podm.common.utils.Collector.toSingle;
import static java.lang.String.format;
import static java.net.URI.create;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
public class EthernetSwitchPortObtainer {
    @Inject
    private EthernetSwitchPortResourceActionsFactory actionsFactory;

    @Inject
    private EthernetSwitchPortMapper switchPortMapper;

    @Inject
    private EthernetSwitchDao ethernetSwitchDao;

    @Inject
    private EthernetSwitchPortVlanObtainer vlanObtainer;

    @Inject
    private Logger logger;

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public EthernetSwitchPort discoverPort(Id switchId, URI portUri) throws ExternalServiceApiReaderException, ActionException {
        EthernetSwitch currentSwitch = getCurrentEthernetSwitch(switchId);
        ExternalService externalService = currentSwitch.getService();
        if (externalService == null) {
            throw new IllegalStateException("There is no Service associated with selected switch port");
        }
        EthernetSwitchPortResourceActions resourceActions = actionsFactory.create(externalService.getBaseUri());
        EthernetSwitchPortResource switchPortResource = resourceActions.getSwitchPort(portUri);
        EthernetSwitchPort targetSwitchPort = readEthernetSwitchPortResource(externalService, portUri, switchPortResource);
        updatePortMembers(externalService, switchPortResource, targetSwitchPort);
        List<EthernetSwitchPortVlan> vlans = discoverPortVlans(switchPortResource, targetSwitchPort);
        setPrimaryVlanInSwitchPort(switchPortResource, targetSwitchPort, vlans);

        Optional<EthernetSwitchPort> possiblyExistingPort = currentSwitch.getPortCollection().stream()
                .filter(port -> Objects.equals(port.getId(), targetSwitchPort.getId()))
                .findFirst();
        if (!possiblyExistingPort.isPresent()) {
            currentSwitch.addPort(targetSwitchPort);
        }
        targetSwitchPort.getService().setIsDirty(true);
        return targetSwitchPort;
    }

    private EthernetSwitch getCurrentEthernetSwitch(Id switchId) throws ActionException {
        try {
            return ethernetSwitchDao.getOrThrow(switchId);
        } catch (IllegalStateException e) {
            String errorMessage = format("EthernetSwitch %s was removed while performing action on switch port!", switchId);
            logger.w(errorMessage);
            throw new ActionException(errorMessage);
        }
    }

    private EthernetSwitchPort readEthernetSwitchPortResource(ExternalService externalService, URI switchPortUri,
                                                              EthernetSwitchPortResource switchPortResource) {
        URI sourceSwitchPortUri = create(switchPortUri.getPath());
        EthernetSwitchPort targetSwitchPort = externalService.findOrCreate(sourceSwitchPortUri, EthernetSwitchPort.class);
        switchPortMapper.map(switchPortResource, targetSwitchPort);
        return targetSwitchPort;
    }

    private void updatePortMembers(ExternalService externalService, EthernetSwitchPortResource switchPortResource,
                                   EthernetSwitchPort targetSwitchPort) throws ExternalServiceApiReaderException {
        List<URI> portMemberUris = getUrisFromResources(switchPortResource.getPortMembers());
        portMemberUris.stream().forEach(portMemberUri -> {
            EthernetSwitchPort memberSwitchPort = externalService.findOrCreate(portMemberUri, EthernetSwitchPort.class);
            targetSwitchPort.addPortMember(memberSwitchPort);
        });

        targetSwitchPort.getPortMembers().stream()
                .filter(memberPort -> !portMemberUris.contains(memberPort.getSourceUri()))
                .forEach(targetSwitchPort::removePortMember);
    }

    private List<EthernetSwitchPortVlan> discoverPortVlans(EthernetSwitchPortResource portResource, EthernetSwitchPort targetSwitchPort)
            throws ExternalServiceApiReaderException {

        List<URI> vlanUris = getUrisFromResources(portResource.getVlans());
        List<EthernetSwitchPortVlan> vlans = new ArrayList<>();
        for (URI vlanUri : vlanUris) {
            vlans.add(vlanObtainer.discoverNewEthernetSwitchPortVlan(targetSwitchPort, vlanUri));
        }

        return vlans;
    }

    private void setPrimaryVlanInSwitchPort(EthernetSwitchPortResource switchPortResource, EthernetSwitchPort targetPort, List<EthernetSwitchPortVlan> vlans)
            throws ExternalServiceApiReaderException {

        EthernetSwitchPortVlan oldPvid = targetPort.getPrimaryVlan();
        ResourceSupplier potentiallyNewPvid = switchPortResource.getPrimaryVlan();
        if (potentiallyNewPvid == null) {
            if (oldPvid != null) {
                targetPort.removePrimaryVlan(oldPvid);
            }
        } else {
            EthernetSwitchPortVlan newPrimaryVlan = vlans.stream()
                    .filter(vlan -> vlan.getSourceUri().equals(potentiallyNewPvid.getUri()))
                    .collect(toSingle());

            if (!Objects.equals(oldPvid, newPrimaryVlan)) {
                if (oldPvid != null) {
                    targetPort.removePrimaryVlan(oldPvid);
                }
                targetPort.setPrimaryVlan(newPrimaryVlan);
            }
        }
    }
}
