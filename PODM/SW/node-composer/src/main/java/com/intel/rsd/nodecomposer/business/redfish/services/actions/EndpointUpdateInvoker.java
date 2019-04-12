/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.actions.EndpointUpdateRequest;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.EndpointAuthentication;
import com.intel.rsd.nodecomposer.types.actions.EndpointUpdateDefinition;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EndpointUpdateInvoker {
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public EndpointUpdateInvoker(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    @Transactional(MANDATORY)
    public void updateEndpoint(Endpoint endpoint, EndpointUpdateDefinition endpointUpdateDefinition) throws EntityOperationException {
        requiresNonNull(endpoint, "endpoint");

        EndpointResource endpointResource = performUpdateEndpointAction(endpointUpdateDefinition, endpoint.getUri().toUri());

        EndpointAuthentication authentication = new EndpointAuthentication();
        authentication.setUsername(endpointResource.getAuthentication().getUsername());
        authentication.setPassword(endpointResource.getAuthentication().getPassword());

        endpoint.setAuthentication(authentication);
    }

    private EndpointResource performUpdateEndpointAction(EndpointUpdateDefinition endpointUpdateDefinition, URI sourceUri)
        throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            return webClient.patchAndRetrieve(sourceUri, new EndpointUpdateRequest(endpointUpdateDefinition));
        } catch (WebClientRequestException e) {
            String errorMessage = "Update on selected Endpoint failed";
            log.warn("{} on [ path: {} ]", errorMessage, sourceUri);
            throw new EntityOperationException(errorMessage, e);
        }
    }
}
