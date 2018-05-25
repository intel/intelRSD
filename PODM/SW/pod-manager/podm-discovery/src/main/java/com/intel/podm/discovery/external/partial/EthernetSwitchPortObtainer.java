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

package com.intel.podm.discovery.external.partial;

import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.EthernetSwitchPortResource;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.EthernetSwitchPortMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.client.reader.ResourceSupplier.getUrisFromResources;
import static com.intel.podm.common.utils.Collector.toSingle;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortObtainer {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchPortMapper switchPortMapper;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private EthernetSwitchPortVlanObtainer vlanObtainer;

    @Transactional(MANDATORY)
    public EthernetSwitchPort discoverPort(ExternalService service, URI portUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            EthernetSwitchPortResource switchPortResource = (EthernetSwitchPortResource) webClient.get(portUri);
            EthernetSwitchPort targetSwitchPort = readEthernetSwitchPortResource(service, switchPortResource);
            updatePortMembers(webClient, switchPortResource, targetSwitchPort);

            Set<EthernetSwitchPortVlan> vlans = vlanObtainer.discoverEthernetSwitchPortVlans(service, getUrisFromResources(switchPortResource.getVlans()));
            vlans.forEach(targetSwitchPort::addEthernetSwitchPortVlan);

            setPrimaryVlanInSwitchPort(switchPortResource, targetSwitchPort, vlans);

            return targetSwitchPort;
        }
    }

    private EthernetSwitchPort readEthernetSwitchPortResource(ExternalService service, EthernetSwitchPortResource resource) {
        Id entityId = resource.getGlobalId(service.getId());
        EthernetSwitchPort targetSwitchPort = discoverableEntityDao.findOrCreateEntity(service, entityId, resource.getUri(), EthernetSwitchPort.class);
        switchPortMapper.map(resource, targetSwitchPort);

        return targetSwitchPort;
    }

    private void updatePortMembers(WebClient webClient, EthernetSwitchPortResource switchPortResource,
                                   EthernetSwitchPort targetSwitchPort) throws WebClientRequestException {
        ExternalService externalService = targetSwitchPort.getService();
        Set<URI> portMemberUris = getUrisFromResources(switchPortResource.getPortMembers());
        for (URI portMemberUri : portMemberUris) {
            EthernetSwitchPortResource memberSwitchPortResource = (EthernetSwitchPortResource) webClient.get(portMemberUri);
            EthernetSwitchPort ethernetSwitchPort = readEthernetSwitchPortResource(externalService, memberSwitchPortResource);
            targetSwitchPort.addPortMember(ethernetSwitchPort);
        }

        targetSwitchPort.uncouplePortMembers(portMember -> !portMemberUris.contains(portMember.getSourceUri()));
    }

    private void setPrimaryVlanInSwitchPort(EthernetSwitchPortResource switchPortResource, EthernetSwitchPort targetPort, Set<EthernetSwitchPortVlan> vlans)
        throws WebClientRequestException {

        EthernetSwitchPortVlan oldPvid = targetPort.getPrimaryVlan();
        ResourceSupplier potentiallyNewPvid = switchPortResource.getPrimaryVlan();
        if (potentiallyNewPvid == null) {
            targetPort.setPrimaryVlan(null);
        } else {
            EthernetSwitchPortVlan newPrimaryVlan = vlans.stream()
                .filter(vlan -> vlan.getSourceUri().equals(potentiallyNewPvid.getUri()))
                .collect(toSingle());

            if (!Objects.equals(oldPvid, newPrimaryVlan)) {
                targetPort.setPrimaryVlan(newPrimaryVlan);
            }
        }
    }
}
