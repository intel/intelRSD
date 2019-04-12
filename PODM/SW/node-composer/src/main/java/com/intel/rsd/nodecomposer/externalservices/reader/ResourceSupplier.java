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

package com.intel.rsd.nodecomposer.externalservices.reader;

import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;

import java.net.URI;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;

public interface ResourceSupplier {
    static Set<URI> getUrisFromResources(Iterable<ResourceSupplier> resources) {
        return stream(resources.spliterator(), false)
            .map(ResourceSupplier::getUri)
            .collect(toSet());
    }

    ExternalServiceResource get() throws WebClientRequestException;
    URI getUri();
}
