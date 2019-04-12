/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.UnexpectedRedirectionException;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ServiceRootResource;
import com.intel.rsd.nodecomposer.types.discovery.DiscoveryServiceEndpoint;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.externalservices.WebClientExceptionUtils.tryGetUnderlyingRedirectionException;
import static org.apache.commons.lang3.StringUtils.stripEnd;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

/**
 * Service Detection implementation based on obtaining UUID of service
 * being detected at given URI using REST client.
 */
@Component
@Scope(SCOPE_SINGLETON)
public class ServiceDescriptor {
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public ServiceDescriptor(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    public DiscoveryServiceEndpoint describe(URI serviceUri) throws UnrecognizedServiceException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            ServiceRootResource serviceRoot = fetchServiceRoot(webClient, serviceUri);
            return new DiscoveryServiceEndpoint(serviceRoot.getUuid(), serviceUri.resolve(serviceRoot.getUri()));
        } catch (WebClientRequestException e) {
            throw new UnrecognizedServiceException("Unable to retrieve Rack Scale service root for: " + serviceUri, e);
        }
    }

    private ServiceRootResource fetchServiceRoot(WebClient webClient, URI serviceRootUri) throws WebClientRequestException {
        try {
            return getServiceRootResource(webClient, serviceRootUri);
        } catch (WebClientRequestException e) {
            Optional<UnexpectedRedirectionException> redirectionException = tryGetUnderlyingRedirectionException(e);
            if (redirectionException.isPresent() && isValidRedfishServiceRootRedirect(redirectionException.get())) {
                URI redirectUri = redirectionException.get().getRedirectUri();
                try (WebClient webClientForRedirect = webClientBuilder.newSimpleInstance(redirectUri).retryable().build()) {
                    return getServiceRootResource(webClientForRedirect, redirectUri);
                }
            } else {
                throw e;
            }
        }
    }

    private ServiceRootResource getServiceRootResource(WebClient webClient, URI serviceRootUri) throws WebClientRequestException {
        ServiceRootResource serviceRootResource = (ServiceRootResource) webClient.get(URI.create(serviceRootUri.getPath()));
        serviceRootResource.setUri(serviceRootUri);
        return serviceRootResource;
    }

    private boolean isValidRedfishServiceRootRedirect(UnexpectedRedirectionException e) {
        URI redirectUri = e.getRedirectUri();
        if (redirectUri == null) {
            return false;
        }

        return Objects.equals(
            stripEnd(e.getExpectedUri().toString(), "/"),
            stripEnd(redirectUri.toString(), "/")
        );
    }
}
