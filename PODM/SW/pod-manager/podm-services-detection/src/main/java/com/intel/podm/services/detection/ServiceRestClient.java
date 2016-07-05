/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.services.detection;

import com.intel.podm.security.providers.SslContextProvider;
import org.jboss.resteasy.client.jaxrs.ResteasyClient;
import org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder;

import javax.enterprise.context.RequestScoped;
import javax.ws.rs.ProcessingException;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.Response;
import java.net.URI;
import java.util.HashMap;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.LUI;
import static java.lang.String.format;
import static java.net.URI.create;
import static java.util.UUID.fromString;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON_TYPE;
import static javax.ws.rs.core.Response.Status.Family.SUCCESSFUL;
import static org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder.HostnameVerificationPolicy.ANY;

@RequestScoped
public class ServiceRestClient {

    private static final String UUID_JSON_PROPERTY = "UUID";
    private static final long CLIENT_TIMEOUT_SECONDS = 2L;

    private static final String LUI_COMPUTER_SYSTEM_URI_PART = "/Systems/1";

    /**
     * Method tries to obtain UUID from body of HTTP response
     * returned from GET request on given URI of a service.
     *
     * Method throws if service is not available, e.g.:
     * - returns HTTP status other than SUCCESSFUL (status code 2xx)
     * - service URI is unreachable (within 2 seconds timeout)
     * - an error occurred during examining response body
     *
     * @param candidate Candidate service containing URI to be examined if it qualifies for discovery
     * @return UUID of examined service
     * @throws ServiceDetectionException
     */
    public UUID obtainServiceUuidFromCandidate(ServiceEndpointCandidate candidate) throws ServiceDetectionException {
        UUID serviceUuid = obtainServiceUuidFromUri(candidate.getEndpointUri());
        if (candidate.getServiceType() == LUI) {
            URI computeModulesUri = create(candidate.getEndpointUri().toString() + LUI_COMPUTER_SYSTEM_URI_PART);
            if (isLuiComputeModulePresent(computeModulesUri)) {
                return serviceUuid;
            } else {
                throw new ServiceDetectionException("LUI Service is not ready yet...");
            }
        } else {
            return serviceUuid;
        }
    }

    public UUID obtainServiceUuidFromService(ServiceEndpoint service) throws ServiceDetectionException {
        return obtainServiceUuidFromUri(service.getEndpointUri());
    }

    private UUID obtainServiceUuidFromUri(URI candidate) throws ServiceDetectionException {
        ResteasyClient client = getResteasyClient();
        try {
            Response response = doGetResponse(client, candidate);

            if (isResponseSuccessful(response)) {
                HashMap<String, String> hashMap = response.readEntity(HashMap.class);
                response.close();
                String uuid = hashMap.get(UUID_JSON_PROPERTY);
                if (uuid == null) {
                    throw new ServiceDetectionException("URI unavailable: " + candidate + " UUID property is missing");
                }
                return fromString(uuid);
            } else {
                throw new WebApplicationException(response);
            }
        } catch (IllegalArgumentException | ProcessingException | WebApplicationException e) {
            throw new ServiceDetectionException(format("Could not obtain UUID from URI: %s, %s", candidate, e.getMessage()));
        } finally {
            client.close();
        }
    }

    private boolean isLuiComputeModulePresent(URI computeModuleUri) throws ServiceDetectionException {
        ResteasyClient client = getResteasyClient();
        try {
            Response computeModuleResponse = doGetResponse(client, computeModuleUri);
            computeModuleResponse.close();
            return isResponseSuccessful(computeModuleResponse);
        } catch (ProcessingException | WebApplicationException e) {
            throw new ServiceDetectionException("URI unavailable: " + computeModuleUri.toString());
        } finally {
            client.close();
        }
    }

    private boolean isResponseSuccessful(Response response) {
        return response.getStatusInfo().getFamily() == SUCCESSFUL;
    }

    private Response doGetResponse(ResteasyClient client, URI uri) {
        WebTarget rootTarget = client.target(uri);
        return rootTarget.request(APPLICATION_JSON_TYPE).get();
    }

    private ResteasyClient getResteasyClient() {
        ResteasyClientBuilder clientBuilder = ((ResteasyClientBuilder) ResteasyClientBuilder.newBuilder())
                .connectionPoolSize(2)
                .connectionTTL(CLIENT_TIMEOUT_SECONDS, SECONDS)
                .establishConnectionTimeout(CLIENT_TIMEOUT_SECONDS, SECONDS)
                .sslContext(SslContextProvider.getContext())
                .hostnameVerification(ANY)
                .socketTimeout(CLIENT_TIMEOUT_SECONDS, SECONDS);
        return clientBuilder.build();
    }
}
