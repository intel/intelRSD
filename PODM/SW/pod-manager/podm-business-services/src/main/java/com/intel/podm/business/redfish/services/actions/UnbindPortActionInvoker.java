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

package com.intel.podm.business.redfish.services.actions;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.AclPortActionRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.partial.EthernetSwitchAclObtainer;
import com.intel.podm.discovery.external.partial.EthernetSwitchPortObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class UnbindPortActionInvoker {
    private static final String UNBIND_ACTION_SUFFIX = "/Actions/EthernetSwitchACL.Unbind";
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchAclObtainer aclObtainer;

    @Inject
    private EthernetSwitchPortObtainer portObtainer;

    @Transactional(MANDATORY)
    public void unbindPortFromAcl(EthernetSwitchAcl acl, EthernetSwitchPort port) throws BusinessApiException {
        ExternalService service = acl.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected ACL");

        URI serviceBaseUri = service.getBaseUri();
        URI aclUri = acl.getSourceUri();

        URI unbindActionUri = URI.create(aclUri + UNBIND_ACTION_SUFFIX);
        URI portUri = port.getSourceUri();

        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            webClient.post(unbindActionUri, new AclPortActionRequest(portUri));
        } catch (WebClientRequestException e) {
            String errorMessage = format("Unbinding Port %s from ACL %s failed", port.getId(), acl.getId());
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, aclUri);
            throw new BusinessApiException(errorMessage, e);
        }

        obtainUpdatedResources(service, aclUri, portUri);
    }

    private void obtainUpdatedResources(ExternalService service, URI aclUri, URI portUri) throws BusinessApiException {
        try {
            aclObtainer.discoverAcl(service, aclUri);
            portObtainer.discoverPort(service, portUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "Unbinding Port was successful, but failed on refreshing resource";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", service.getBaseUri(), e.getResourceUri());
            throw new BusinessApiException(errorMessage, e);
        }
    }
}
