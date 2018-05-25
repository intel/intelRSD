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
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.EthernetSwitchAclRuleResource;
import com.intel.podm.common.types.Id;
import com.intel.podm.mappers.redfish.EthernetSwitchAclRuleMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Set;

import static com.intel.podm.client.reader.ResourceSupplier.getUrisFromResources;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchAclRuleObtainer {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private EthernetSwitchAclRuleMapper mapper;

    @Inject
    private EthernetSwitchPortObtainer ethernetSwitchPortObtainer;

    @Transactional(MANDATORY)
    public EthernetSwitchAclRule discoverEthernetSwitchAclRule(ExternalService service, URI ruleUri) throws WebClientRequestException {
        requiresNonNull(service, "service", "There is no Service associated with selected ACL rule");

        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            EthernetSwitchAclRuleResource psmeRule = (EthernetSwitchAclRuleResource) webClient.get(ruleUri);

            Id entityId = psmeRule.getGlobalId(service.getId());
            EthernetSwitchAclRule target = discoverableEntityDao.findOrCreateEntity(service, entityId, psmeRule.getUri(), EthernetSwitchAclRule.class);
            updateMirrorPorts(psmeRule, target);
            mapper.map(psmeRule, target);

            return target;
        }
    }

    private void updateMirrorPorts(EthernetSwitchAclRuleResource ruleResource, EthernetSwitchAclRule targetRule) throws WebClientRequestException {
        ExternalService externalService = targetRule.getService();
        ResourceSupplier forwardMirrorInterface = ruleResource.getForwardMirrorInterface();
        if (forwardMirrorInterface != null) {
            URI portUri = forwardMirrorInterface.getUri();
            EthernetSwitchPort forwardMirrorInterfacePort = ethernetSwitchPortObtainer.discoverPort(externalService, portUri);
            targetRule.setForwardMirrorInterface(forwardMirrorInterfacePort);
        } else {
            targetRule.setForwardMirrorInterface(null);
        }

        Set<URI> mirrorPortsUris = getUrisFromResources(ruleResource.getMirrorPortRegion());
        for (URI mirrorPortUri : mirrorPortsUris) {
            EthernetSwitchPort port = ethernetSwitchPortObtainer.discoverPort(externalService, mirrorPortUri);
            targetRule.addMirrorPort(port);
        }
        targetRule.uncoupleMirrorPortRegion(portMember -> !mirrorPortsUris.contains(portMember.getSourceUri()));
    }
}
