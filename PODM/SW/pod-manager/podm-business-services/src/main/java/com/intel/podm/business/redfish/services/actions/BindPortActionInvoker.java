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
public class BindPortActionInvoker {
    @Inject
    private Logger logger;

    @Inject
    private EthernetSwitchAclObtainer aclObtainer;

    @Inject
    private EthernetSwitchPortObtainer portObtainer;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Transactional(MANDATORY)
    public void bindPortToAcl(EthernetSwitchAcl acl, EthernetSwitchPort port) throws BusinessApiException {
        ExternalService service = acl.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected ACL");

        URI serviceBaseUri = service.getBaseUri();
        URI aclSourceUri = acl.getSourceUri();
        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            URI bindActionUri = URI.create(aclSourceUri + "/Actions/EthernetSwitchACL.Bind");
            webClient.post(bindActionUri, new AclPortActionRequest(port.getSourceUri()));
        } catch (WebClientRequestException e) {
            String errorMessage = format("Binding Port %s to ACL %s failed", port.getId(), acl.getId());
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, aclSourceUri);
            throw new BusinessApiException(errorMessage, e);
        }

        try {
            aclObtainer.discoverAcl(service, aclSourceUri);
            portObtainer.discoverPort(service, port.getSourceUri());
        } catch (WebClientRequestException e) {
            String errorMessage = "Binding Port was successful, but failed on refreshing resource";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, e.getResourceUri());
            throw new BusinessApiException(errorMessage, e);
        }
    }
}
