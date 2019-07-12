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

package com.intel.rsd.nodecomposer.services.configuration.client;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class DiscoveryServiceWebClient {

    private static final String EXCEPTION_LOG_TEMPLATE = "%s on [service: %s, path: %s] %n";
    private final String basicAuthToken;
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public DiscoveryServiceWebClient(@Value("${discovery-service.basicAuthToken:#{null}}") String basicAuthToken, WebClientBuilder webClientBuilder) {
        this.basicAuthToken = basicAuthToken;
        this.webClientBuilder = webClientBuilder;
    }

    public URI post(URI baseUri, URI requestUri, Object request) {
        try (WebClient webClient = createWebClient(baseUri)) {
            return webClient.post(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = "POST action failed on selected uri";
            log.warn(format(EXCEPTION_LOG_TEMPLATE, errorMessage, baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }

    public void delete(URI baseUri, URI requestUri) {
        requiresNonNull(baseUri, "baseUri");
        requiresNonNull(requestUri, "requestUri");

        try (WebClient webClient = createWebClient(baseUri)) {
            webClient.delete(requestUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "DELETE action failed on selected uri";
            log.warn(format(EXCEPTION_LOG_TEMPLATE, errorMessage, baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }

    public void patch(URI baseUri, URI requestUri, Object request) {
        try (WebClient webClient = createWebClient(baseUri)) {
            webClient.patch(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = "PATCH action failed on selected uri";
            log.warn(format(EXCEPTION_LOG_TEMPLATE, errorMessage, baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }

    public WebClient createWebClient(URI baseUri) {
        return webClientBuilder.newSimpleInstance(baseUri).retryable().withBasicAuthToken(basicAuthToken).build();
    }
}
