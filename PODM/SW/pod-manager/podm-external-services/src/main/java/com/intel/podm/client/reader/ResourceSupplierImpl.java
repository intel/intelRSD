/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.reader;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;

import java.net.URI;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;


/**
 * Generic ResourceSupplier implementation that uses HTTP to lazy load Resources
 */
public final class ResourceSupplierImpl implements ResourceSupplier {
    private final URI uri;
    private final WebClient webClient;

    public ResourceSupplierImpl(WebClient webClient, URI uri) {
        this.webClient = webClient;
        this.uri = uri;
    }

    @Override
    public ExternalServiceResource get() throws ExternalServiceApiReaderException {
        ExternalServiceResource resource = webClient.get(uri);
        if (uri.getFragment() != null) {
            resource = resource.getByFragment(uri.getFragment());
            requiresNonNull(resource, () -> new ExternalServiceApiReaderException("Requested resource doesn't exist", uri));
            resource.setWebClient(webClient);
        }
        resource.setUri(uri);
        return resource;
    }

    @Override
    public URI getUri() {
        return uri;
    }
}
