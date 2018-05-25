/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.redfish.EthernetSwitchAclResource;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.EthernetSwitchAclMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static com.intel.podm.client.reader.ResourceSupplier.getUrisFromResources;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchAclObtainer {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchAclMapper aclMapper;

    @Inject
    private EthernetSwitchPortObtainer ethernetSwitchPortObtainer;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Transactional(MANDATORY)
    public EthernetSwitchAcl discoverAcl(ExternalService service, URI aclUri) throws WebClientRequestException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            EthernetSwitchAclResource aclResource = (EthernetSwitchAclResource) webClient.get(aclUri);
            EthernetSwitchAcl acl = readEthernetSwitchAcl(service, aclResource);

            updateBindActionAllowableValues(aclResource, acl);
            updateBoundPorts(aclResource, acl);

            return acl;
        }
    }

    private EthernetSwitchAcl readEthernetSwitchAcl(ExternalService service, EthernetSwitchAclResource aclResource) {
        Id entityId = aclResource.getGlobalId(service.getId());
        EthernetSwitchAcl acl = discoverableEntityDao.findOrCreateEntity(service, entityId, aclResource.getUri(), EthernetSwitchAcl.class);
        aclMapper.map(aclResource, acl);
        return acl;
    }

    private void updateBindActionAllowableValues(EthernetSwitchAclResource aclResource, EthernetSwitchAcl acl) throws WebClientRequestException {
        ExternalService externalService = acl.getService();
        Set<URI> urisFromResources = getUrisFromResources(aclResource.getBindActionAllowableValues());
        for (URI bindablePortUri : urisFromResources) {
            EthernetSwitchPort bindablePort = ethernetSwitchPortObtainer.discoverPort(externalService, bindablePortUri);
            if (bindablePort != null) {
                acl.addBindActionAllowableValue(bindablePort);
            }
        }

        acl.uncoupleBindActionAllowableValues(port -> !urisFromResources.contains(port.getSourceUri()));
    }

    private void updateBoundPorts(EthernetSwitchAclResource aclResource, EthernetSwitchAcl acl) throws WebClientRequestException {
        ExternalService externalService = acl.getService();
        Set<URI> urisFromResources = getUrisFromResources(aclResource.getBoundPorts());

        for (URI boundPortUri : urisFromResources) {
            EthernetSwitchPort boundPort = ethernetSwitchPortObtainer.discoverPort(externalService, boundPortUri);
            if (boundPort != null) {
                acl.addBoundPort(boundPort);
            }
        }

        acl.uncoupleBoundPorts(port -> !urisFromResources.contains(port.getSourceUri()));
    }
}
