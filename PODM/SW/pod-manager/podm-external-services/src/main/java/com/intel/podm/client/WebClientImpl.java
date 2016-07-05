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

package com.intel.podm.client;

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderConnectionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.redfish.RedfishErrorResponseImpl;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;

import javax.ws.rs.ProcessingException;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.client.Client;
import javax.ws.rs.core.Response;
import java.net.ConnectException;
import java.net.URI;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static javax.ws.rs.client.Entity.json;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON_TYPE;
import static javax.ws.rs.core.Response.Status.Family.SUCCESSFUL;
import static javax.ws.rs.core.UriBuilder.fromUri;
import static org.apache.commons.lang.exception.ExceptionUtils.getRootCause;

/**
 * Web client for REST API
 * Each instance of WebClient uses one JAX-RS client for all its requests
 */
public class WebClientImpl implements WebClient {
    private static final Logger LOGGER = getLogger(WebClientImpl.class);
    private static final String HTTP_METHOD_PATCH = "PATCH";
    private final URI baseUri;
    private final Client client;

    public WebClientImpl(URI baseUri) {
        this.baseUri = baseUri;
        client = ResteasyClientProvider.getClientWithJacksonProvider();
    }

    @Override
    public URI getBaseUri() {
        return baseUri;
    }

    /**
     * Creates HTTP GET Request, waits for response and deserializes it.
     * Request Uri is built based on baseUri (host, scheme, port) and provided uri (rest of uri)
     *
     * @param uri provided URI part
     * @return deserialized response
     * @throws ExternalServiceApiReaderException
     */
    @Override
    public ExternalServiceResource get(URI uri) throws ExternalServiceApiReaderException {
        URI requestUri = buildUri(uri);
        ExternalServiceResource resource = doGetRequest(requestUri);
        resource.setWebClient(this);
        return resource;
    }

    private URI buildUri(URI uri) {
        return fromUri(uri)
                .host(this.baseUri.getHost())
                .scheme(this.baseUri.getScheme())
                .port(this.baseUri.getPort())
                .build();
    }

    private ExternalServiceResource doGetRequest(URI requestUri) throws ExternalServiceApiReaderException {
        try {
            return client
                    .target(requestUri)
                    .request(APPLICATION_JSON_TYPE)
                    .get(ExternalServiceResourceImpl.class);
        } catch (ProcessingException | WebApplicationException e) {
            checkConnectException(requestUri, e);
            if (e instanceof WebApplicationException) {
                WebApplicationException exception = (WebApplicationException) e;
                RedfishErrorResponse redfishErrorResponse = getRedfishErrorResponse(exception.getResponse());
                throw new ExternalServiceApiReaderException(exception.getMessage(), requestUri, e.getCause(), redfishErrorResponse);
            }
            throw new ExternalServiceApiReaderException(e.getMessage(), requestUri, e.getCause());
        }
    }

    private void checkConnectException(URI requestUri, Throwable e) throws ExternalServiceApiReaderConnectionException {
        if (getRootCause(e) instanceof ConnectException) {
            throw new ExternalServiceApiReaderConnectionException(e.getMessage(), requestUri, e.getCause());
        }
    }

    private RedfishErrorResponse getRedfishErrorResponse(Response response) {
        try {
            return response.readEntity(RedfishErrorResponseImpl.class);
        } catch (ProcessingException | IllegalStateException ex) {
            LOGGER.w("Unable to parse RedfishError for: {}", response.getLocation());
            // TODO build detailed RedfishError with specific code / message here
            return null;
        }
    }

    /**
     * Creates HTTP PATCH Request with obj serialized to JSON as its body
     * Request Uri is built based on baseUri (host, scheme, port) and provided uri (rest of uri)
     *
     * @param uri provided URI part
     * @param obj PATCH Request entity
     * @param <T> PATCH Request entity type
     * @throws ExternalServiceApiActionException
     */
    @Override
    public <T> void patch(URI uri, T obj) throws ExternalServiceApiActionException {
        validateParameters(uri, obj);
        URI requestUri = buildUri(uri);
        try {
            Response response = client
                    .target(requestUri)
                    .request(APPLICATION_JSON_TYPE)
                    .build(HTTP_METHOD_PATCH, json(obj))
                    .invoke();
            if (isUnsuccessful(response)) {
                RedfishErrorResponse redfishErrorResponse = getRedfishErrorResponse(response);
                int statusCode = response.getStatus();
                response.close();
                throw new ExternalServiceApiActionException("PATCH action was not successful (status: " + statusCode + ")", requestUri, redfishErrorResponse);
            } else {
                response.close();
            }
        } catch (ProcessingException e) {
            throw new ExternalServiceApiActionException(e.getMessage(), requestUri, e.getCause());
        }
    }

    private <T> void validateParameters(URI requestUri, T obj) {
        if (requestUri == null) {
            throw new IllegalArgumentException("requestUri should not be null");
        }

        if (obj == null) {
            throw new IllegalArgumentException("obj should not be null");
        }
    }

    private boolean isUnsuccessful(Response response) {
        return response.getStatusInfo().getFamily() != SUCCESSFUL;
    }

    /**
     * Creates HTTP POST Request with obj serialized to JSON as its body
     * Request Uri is built based on baseUri (host, scheme, port) and provided uri (rest of uri)
     *
     * @param <T> POST Request entity type
     * @param uri provided URI part
     * @param obj POST Request entity
     * @return URI Location header of response
     * @throws ExternalServiceApiActionException
     */
    @Override
    public <T> URI post(URI uri, T obj) throws ExternalServiceApiActionException {
        validateParameters(uri, obj);
        URI requestUri = buildUri(uri);
        try {
            Response response = client
                    .target(requestUri)
                    .request(APPLICATION_JSON_TYPE)
                    .post(json(obj));
            if (isUnsuccessful(response)) {
                RedfishErrorResponse redfishErrorResponse = getRedfishErrorResponse(response);
                int statusCode = response.getStatus();
                response.close();
                throw new ExternalServiceApiActionException("POST action was not successful (status: " + statusCode + ")", requestUri, redfishErrorResponse);
            } else {
                URI location = response.getLocation();
                response.close();
                return location;
            }
        } catch (ProcessingException e) {
            throw new ExternalServiceApiActionException(e.getMessage(), requestUri, e.getCause());
        }
    }

    /**
     * Creates HTTP DELETE Request. Request Uri is built based on baseUri (host, scheme, port)
     * and provided uri (rest of uri)
     *
     * @param uri URI of resource to delete
     * @throws ExternalServiceApiActionException
     */
    @Override
    public void delete(URI uri) throws ExternalServiceApiActionException {
        if (uri == null) {
            throw new IllegalArgumentException("requestUri should not be null");
        }
        URI requestUri = buildUri(uri);
        try {
            Response response = client
                    .target(requestUri)
                    .request(APPLICATION_JSON_TYPE)
                    .delete();
            if (isUnsuccessful(response)) {
                RedfishErrorResponse redfishErrorResponse = getRedfishErrorResponse(response);
                int statusCode = response.getStatus();
                response.close();
                throw new ExternalServiceApiActionException("DELETE action was not successful (status: " + statusCode + ")",
                        requestUri, redfishErrorResponse);
            } else {
                response.close();
            }
        } catch (ProcessingException e) {
            throw new ExternalServiceApiActionException(e.getMessage(), requestUri, e.getCause());
        }
    }

    /**
     * Releases all resources related to this WebClient
     */
    @Override
    public void close() {
        client.close();
    }

    public static WebClient createRetryable(URI baseUri) {
        return new WebClientWithRetrying(new WebClientImpl(baseUri));
    }
}
