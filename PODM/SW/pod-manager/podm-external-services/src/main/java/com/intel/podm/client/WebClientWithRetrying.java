/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.resources.ExternalServiceResource;

import java.net.URI;

import static com.google.common.base.Preconditions.checkNotNull;

class WebClientWithRetrying implements WebClient {
    private final WebClient innerClient;

    WebClientWithRetrying(WebClient client) {
        checkNotNull(client);
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
    public ExternalServiceResource get(URI uri) throws ExternalServiceApiReaderException {
        return RetryInvoker.retry(() -> innerClient.get(uri));
    }

    @Override
    public <T> URI post(URI requestUri, T obj) throws ExternalServiceApiActionException {
        return RetryInvoker.retry(() -> innerClient.post(requestUri, obj));
    }

    @Override
    public <T> void patch(URI requestUri, T obj) throws ExternalServiceApiActionException {
        RetryInvoker.retry(() -> innerClient.patch(requestUri, obj));
    }

    @Override
    public void delete(URI requestUri) throws ExternalServiceApiActionException {
        RetryInvoker.retry(() -> innerClient.delete(requestUri));
    }
}
