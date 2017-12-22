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
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.CreateAclRequest;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.partial.EthernetSwitchAclObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EthernetSwitchAclActionsInvoker {
    @Inject
    private Logger logger;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private EthernetSwitchAclObtainer ethernetSwitchAclObtainer;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    @Transactional(MANDATORY)
    public EthernetSwitchAcl createAcl(EthernetSwitch currentSwitch) throws EntityOperationException {
        ExternalService service = currentSwitch.getService();
        requiresNonNull(service, "service", "There is no Service associated with selected switch");

        URI serviceBaseUri = service.getBaseUri();
        URI aclCollectionUri = URI.create(currentSwitch.getSourceUri() + "/ACLs");

        URI newAclUri = performCreateAclAction(serviceBaseUri, aclCollectionUri);
        EthernetSwitchAcl ethernetSwitchAcl = obtainNewlyCreatedAcl(service, serviceBaseUri, newAclUri);

        currentSwitch.addAcl(ethernetSwitchAcl);
        return ethernetSwitchAcl;
    }

    private URI performCreateAclAction(URI serviceBaseUri, URI aclCollectionUri) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(serviceBaseUri).retryable().build()) {
            return webClient.post(aclCollectionUri, new CreateAclRequest());

        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchAcl creation failed on selected switch";
            logger.w(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, aclCollectionUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    private EthernetSwitchAcl obtainNewlyCreatedAcl(ExternalService service, URI serviceBaseUri, URI newAclUri) throws EntityOperationException {
        try {
            return ethernetSwitchAclObtainer.discoverAcl(service, newAclUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "EthernetSwitchAcl creation was successful, but failed on refreshing selected ACL";
            logger.i(errorMessage + " on [ service: {}, path: {} ]", serviceBaseUri, e.getResourceUri());
            throw new EntityOperationException(errorMessage, e);
        }
    }

    @Transactional(MANDATORY)
    public void deleteAcl(EthernetSwitchAcl ethernetSwitchAcl) throws EntityOperationException {
        ExternalService service = ethernetSwitchAcl.getService();
        requiresNonNull(service, () -> new IllegalStateException("There is no Service associated with selected switch"));
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            webClient.delete(ethernetSwitchAcl.getSourceUri());
        } catch (WebClientRequestException e) {
            logger.d(e.getMessage(), e);
            throw new EntityOperationException("Cannot delete requested ACL", e);
        }
        discoverableEntityDao.removeWithConnectedExternalLinks(ethernetSwitchAcl);
    }
}
