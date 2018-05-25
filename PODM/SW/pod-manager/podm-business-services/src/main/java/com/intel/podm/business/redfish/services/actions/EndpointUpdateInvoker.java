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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.embeddables.EndpointAuthentication;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.actions.EndpointUpdateRequest;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.EndpointUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EndpointUpdateInvoker {
    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public void updateEndpoint(Endpoint endpoint, EndpointUpdateDefinition endpointUpdateDefinition) throws EntityOperationException {
        requiresNonNull(endpoint, "endpoint");
        ExternalService service = endpoint.getService();
        requiresNonNull(service, "service ", "there is no Service associated with selected Endpoint");

        EndpointResource endpointResource = performUpdateEndpointAction(endpointUpdateDefinition, service, endpoint.getSourceUri());

        EndpointAuthentication authentication = new EndpointAuthentication();
        authentication.setUsername(endpointResource.getAuthentication().getUsername());
        authentication.setPassword(endpointResource.getAuthentication().getPassword());

        endpoint.setAuthentication(authentication);
    }

    private EndpointResource performUpdateEndpointAction(EndpointUpdateDefinition endpointUpdateDefinition, ExternalService service, URI sourceUri)
        throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.newInstance(service.getBaseUri()).retryable().build()) {
            return webClient.patchAndRetrieve(sourceUri, new EndpointUpdateRequest(endpointUpdateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Endpoint failed";
            logger.w("{} on [ service: {}, path: {} ]", errorMessage, service, sourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
