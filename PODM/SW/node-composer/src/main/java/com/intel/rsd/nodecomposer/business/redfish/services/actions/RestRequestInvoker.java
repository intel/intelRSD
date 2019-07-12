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
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;

import static java.lang.String.format;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class RestRequestInvoker {
    private final WebClientBuilder webClientBuilder;

    @Autowired
    public RestRequestInvoker(WebClientBuilder webClientBuilder) {
        this.webClientBuilder = webClientBuilder;
    }

    public URI post(URI requestUri, Object request) throws EntityOperationException {
        return post(requestUri, request, webClientBuilder.createResourceManagerInstance().build());
    }

    public URI postWithRetry(URI requestUri, Object request) throws EntityOperationException {
        return post(requestUri, request, webClientBuilder.createResourceManagerInstance().retryable().build());
    }

    public void patchWithRetry(URI requestUri, Object request) throws EntityOperationException {
        try (WebClient webClient = webClientBuilder.createResourceManagerInstance().retryable().build()) {
            webClient.patch(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = format("Operation PATCH failed on selected uri [ path: %s ]", requestUri);
            log.warn(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

    public void delete(URI requestUri) throws WebClientRequestException {
        webClientBuilder.createResourceManagerInstance().retryable().build().delete(requestUri);
    }

    private URI post(URI requestUri, Object request, WebClient wc) throws EntityOperationException {
        try (WebClient webClient = wc) {
            return webClient.post(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = format("Operation POST failed on selected uri [ path: %s, request %s ]", requestUri, request);
            log.warn(errorMessage);
            throw new EntityOperationException(errorMessage, e);
        }
    }

}
