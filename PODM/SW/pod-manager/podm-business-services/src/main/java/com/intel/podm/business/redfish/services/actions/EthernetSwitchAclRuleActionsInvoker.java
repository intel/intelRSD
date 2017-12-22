/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchAclRule;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.RuleModificationRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.AclRuleModificationRequest;
import com.intel.podm.discovery.external.partial.EthernetSwitchAclRuleObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchAclRuleActionsInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchAclRuleObtainer ruleObtainer;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Inject
    private Logger logger;

    public EthernetSwitchAclRule create(EthernetSwitchAcl acl, AclRuleModificationRequest request) throws EntityOperationException {
        ExternalService service = acl.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected ACL rule");

        URI switchAclUri = acl.getSourceUri();
        URI ruleUri = performCreateRuleAction(request, service, switchAclUri);
        try {
            return ruleObtainer.discoverEthernetSwitchAclRule(service, ruleUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "ACL rule refreshing failed on selected switch ACL";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), ruleUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private URI performCreateRuleAction(AclRuleModificationRequest request, ExternalService service, URI switchAclUri) throws EntityOperationException {
        URI ruleUri;
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            URI rulesCollectionUri = URI.create(switchAclUri + "/Rules");
            ruleUri = webClient.post(rulesCollectionUri, toRuleModificationRequest(request));
        } catch (WebClientRequestException e) {
            String errorMessage = "ACL rule creation failed on selected switch ACL";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), switchAclUri);
            throw new EntityOperationException(errorMessage, e);
        }
        return ruleUri;
    }

    private RuleModificationRequest toRuleModificationRequest(AclRuleModificationRequest request) {
        return new RuleModificationRequest.Builder()
            .setRuleId(request.getRuleId())
            .setAction(request.getAction())
            .setForwardMirrorInterface(request.getForwardMirrorInterface())
            .setMirrorPortRegions(request.getMirrorPortRegion())
            .setMirrorType(request.getMirrorType())
            .setCondition(request.getCondition())
            .build();
    }

    @Transactional(MANDATORY)
    public void deleteAclRule(EthernetSwitchAclRule aclRule) throws EntityOperationException {
        ExternalService service = aclRule.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected ACL rule");

        URI aclRuleSourceUri = aclRule.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.delete(aclRuleSourceUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Selected ACL Rule could not be deleted";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), aclRuleSourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
        discoverableEntityDao.removeWithConnectedExternalLinks(aclRule);
    }

    @Transactional(MANDATORY)
    public void update(EthernetSwitchAclRule ethernetSwitchAclRule, AclRuleModificationRequest request) throws EntityOperationException {
        ExternalService service = ethernetSwitchAclRule.getService();
        requiresNonNull(service, () -> new IllegalStateException("There is no Service associated with selected ACLRule"));

        URI aclRuleUri = ethernetSwitchAclRule.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.patch(aclRuleUri, toRuleModificationRequest(request));
        } catch (WebClientRequestException e) {
            String errorMessage = "Requested ACL rule cannot be updated";
            logger.w(errorMessage + " on [ service: {}, aclRule: {} ]", service.getBaseUri(), aclRuleUri);
            throw new EntityOperationException(errorMessage, e);
        }
        try {
            ruleObtainer.discoverEthernetSwitchAclRule(service, aclRuleUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "ACL rule refreshing failed on selected switch ACL";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), aclRuleUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
