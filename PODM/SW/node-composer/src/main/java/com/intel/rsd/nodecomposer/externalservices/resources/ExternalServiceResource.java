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

package com.intel.rsd.nodecomposer.externalservices.resources;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceLinks;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;

import java.net.URI;
import java.util.stream.StreamSupport;

import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

public interface ExternalServiceResource {
    /**
     * Gets relative URI of resource.
     */
    URI getUri();

    void setUri(URI uri);

    void setWebClient(WebClient webClient);

    ResourceLinks getLinks();

    default Iterable<ResourceSupplier> toSuppliers(WebClient webClient, Iterable<? extends ODataId> collection)
        throws WebClientRequestException {
        if (collection == null) {
            return emptyList();
        }

        return StreamSupport.stream(collection.spliterator(), false)
            .map(odataid -> toSupplier(webClient, odataid))
            .collect(toList());
    }

    default ResourceSupplier toSupplier(WebClient webClient, ODataId oDataId) {
        if (oDataId == null || oDataId.toUri() == null) {
            return new ResourceSupplier() {
                @Override
                public ExternalServiceResource get() throws WebClientRequestException {
                    throw new WebClientRequestException(
                        "null ODataId", ExternalServiceResource.this.getUri(), new IllegalStateException("null ODataId")
                    );
                }

                @Override
                public URI getUri() {
                    throw new IllegalStateException("URI not available");
                }
            };
        }

        return new ResourceSupplierImpl(webClient, oDataId);
    }

    class ResourceSupplierImpl implements ResourceSupplier {
        private final WebClient webClient;
        private final URI uri;

        private ResourceSupplierImpl(WebClient webClient, ODataId oDataId) {
            this.webClient = webClient;
            this.uri = oDataId.toUri();
        }

        @Override
        public ExternalServiceResource get() throws WebClientRequestException {
            return webClient.get(uri);
        }

        @Override
        public URI getUri() {
            return uri;
        }
    }
}
