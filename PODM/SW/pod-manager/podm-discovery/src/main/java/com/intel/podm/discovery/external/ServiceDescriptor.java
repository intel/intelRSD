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

package com.intel.podm.discovery.external;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.redfish.response.UnexpectedRedirectionException;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.common.types.discovery.ServiceEndpoint;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.net.URI;
import java.util.Iterator;
import java.util.Objects;
import java.util.Optional;

import static com.intel.podm.client.WebClientExceptionUtils.tryGetUnderlyingRedirectionException;
import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static org.apache.commons.lang3.StringUtils.stripEnd;

/**
 * Service Detection implementation based on obtaining UUID of service
 * being detected at given URI using REST client.
 */
@ApplicationScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ServiceDescriptor {
    private static final String LUI_COMPUTER_SYSTEM_URI_PART = "/redfish/v1/Systems/1";
    private static final ServiceType UNDEFINED_SERVICE_TYPE = null;

    @Inject
    private WebClientBuilder webClientBuilder;

    @Config
    @Inject
    private Holder<ServiceDetectionConfig> serviceDetectionConfig;

    public ServiceEndpoint describe(URI serviceUri) throws UnrecognizedServiceTypeException {
        return describe(serviceUri, UNDEFINED_SERVICE_TYPE);
    }

    public ServiceEndpoint describe(URI serviceUri, ServiceType predefinedServiceType) throws UnrecognizedServiceTypeException {
        try (WebClient webClient = webClientBuilder.newInstance(serviceUri).retryable().build()) {
            ServiceRootResource serviceRoot = fetchServiceRoot(webClient, serviceUri);
            ServiceType serviceType = determineServiceType(predefinedServiceType, serviceRoot);
            if (LUI.equals(serviceType)) {
                if (!checkIfLuiIsReady(serviceRoot)) {
                    throw new UnrecognizedServiceTypeException("LUI service is not ready yet.");
                }
            }
            return new ServiceEndpoint(serviceType, serviceRoot.getUuid(), serviceUri.resolve(serviceRoot.getUri()));
        } catch (WebClientRequestException e) {
            throw new UnrecognizedServiceTypeException("Unable to retrieve Rack Scale service root for: " + serviceUri, e);
        }
    }

    private ServiceRootResource fetchServiceRoot(WebClient webClient, URI serviceRootUri) throws WebClientRequestException {
        try {
            return getServiceRootResource(webClient, serviceRootUri);
        } catch (WebClientRequestException e) {
            Optional<UnexpectedRedirectionException> redirectionException = tryGetUnderlyingRedirectionException(e);
            if (redirectionException.isPresent() && isValidRedfishServiceRootRedirect(redirectionException.get())) {
                URI redirectUri = redirectionException.get().getRedirectUri();
                try (WebClient webClientForRedirect = webClientBuilder.newInstance(redirectUri).retryable().build()) {
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

    private ServiceType determineServiceType(ServiceType predefinedServiceType, ServiceRootResource redfishService) {
        ServiceType serviceType = predefinedServiceType;
        if (serviceType == UNDEFINED_SERVICE_TYPE) {
            serviceType = serviceDetectionConfig.get().getServiceTypeMapping().getServiceTypeForName(redfishService.getName());
            if (serviceType == null) {
                serviceType = PSME;
            }
        }
        return serviceType;
    }

    private boolean checkIfLuiIsReady(ServiceRootResource redfishService) throws WebClientRequestException {
        Iterator chassisCollection = redfishService.getComputerSystems().iterator();
        if (chassisCollection.hasNext()) {
            ResourceSupplier resourceSupplier = (ResourceSupplier) chassisCollection.next();
            if (resourceSupplier.getUri().toString().equals(LUI_COMPUTER_SYSTEM_URI_PART)) {
                return true;
            }
        }
        return false;
    }
}
