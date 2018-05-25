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

package com.intel.podm.services.configuration.client;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

@ApplicationScoped
public class RestActionInvoker {

    @Inject
    private WebClientBuilder webClientBuilder;

    @Inject
    private Logger logger;

    public URI post(URI baseUri, URI requestUri, Object request) {
        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            return webClient.post(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = "POST action failed on selected uri";
            logger.w(format(errorMessage + " on [service: %s, path: %s] \n", baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }

    public void delete(URI baseUri, URI requestUri) {
        requiresNonNull(baseUri, "baseUri");
        requiresNonNull(requestUri, "requestUri");

        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            webClient.delete(requestUri);
        } catch (WebClientRequestException e) {
            String errorMessage = "DELETE action failed on selected uri";
            logger.w(format(errorMessage + " on [service: %s, path: %s] \n", baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }

    public void patch(URI baseUri, URI requestUri, Object request) {
        try (WebClient webClient = webClientBuilder.newInstance(baseUri).retryable().build()) {
            webClient.patch(requestUri, request);
        } catch (WebClientRequestException e) {
            String errorMessage = "PATCH action failed on selected uri";
            logger.w(format(errorMessage + " on [service: %s, path: %s] \n", baseUri, requestUri), e);

            throw new RuntimeException(errorMessage, e);
        }
    }
}
