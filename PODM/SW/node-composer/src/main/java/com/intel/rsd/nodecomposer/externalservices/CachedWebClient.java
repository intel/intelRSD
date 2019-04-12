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

package com.intel.rsd.nodecomposer.externalservices;

import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;

import java.net.URI;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

final class CachedWebClient implements WebClient {
    private final WebClient decoratedClient;
    private final Object lock = new Object();

    private final Map<URI, ExternalServiceResource> cache = new HashMap<>();

    CachedWebClient(WebClient decoratedClient) {
        this.decoratedClient = decoratedClient;
    }

    @Override
    public ExternalServiceResource get(URI uri) throws WebClientRequestException {
        synchronized (lock) {
            if (cache.containsKey(uri)) {
                ExternalServiceResource resource = cache.get(uri);
                resource.setWebClient(this);
                return resource;
            }

            ExternalServiceResource resource = decoratedClient.get(uri);
            resource.setWebClient(this);
            cache.put(uri, resource);

            return resource;
        }
    }

    @Override
    public void close() {
        synchronized (lock) {
            cache.clear();
            decoratedClient.close();
        }
    }

    @Override
    public <T> Optional<ExternalServiceResource> patch(URI requestUri, T obj) throws WebClientRequestException {
        return decoratedClient.patch(requestUri, obj);
    }

    @Override
    public <T> URI post(URI requestUri, T obj) throws WebClientRequestException {
        return decoratedClient.post(requestUri, obj);
    }

    @Override
    public <T> void postNotMonitored(String requestUri, T obj) throws WebClientRequestException {
        decoratedClient.postNotMonitored(requestUri, obj);
    }

    @Override
    public void delete(URI requestUri) throws WebClientRequestException {
        decoratedClient.delete(requestUri);
    }

    @Override
    public URI getBaseUri() {
        return decoratedClient.getBaseUri();
    }
}
