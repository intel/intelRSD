/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices;

import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;

import java.net.URI;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

class WebClientWithRetrying implements WebClient {
    private final WebClient innerClient;

    WebClientWithRetrying(WebClient client) {
        requiresNonNull(client, "com/intel/rsd/nodecomposer/externalservices");
        innerClient = client;
    }

    @Override
    public URI getBaseUri() {
        return innerClient.getBaseUri();
    }

    @Override
    public void close() {
        innerClient.close();
    }

    @Override
    public ExternalServiceResource get(URI requestUri) throws WebClientRequestException {
        return RetryInvoker.retry(() -> innerClient.get(requestUri));
    }

    @Override
    public <T> URI post(URI requestUri, T obj) throws WebClientRequestException {
        return RetryInvoker.retry(() -> innerClient.post(requestUri, obj));
    }

    @Override
    public <T> void postNotMonitored(String requestUri, T obj) throws WebClientRequestException {
        RetryInvoker.retry(() -> innerClient.postNotMonitored(requestUri, obj));
    }

    @Override
    public <T> Optional<ExternalServiceResource> patch(URI requestUri, T obj) throws WebClientRequestException {
        return RetryInvoker.retry(() -> innerClient.patch(requestUri, obj));
    }

    @Override
    public void delete(URI requestUri) throws WebClientRequestException {
        RetryInvoker.retry(() -> innerClient.delete(requestUri));
    }
}
