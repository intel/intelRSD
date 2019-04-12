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

package com.intel.rsd.resourcemanager.layers.merger;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.common.resources.ResourceProvider;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;

import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;

@Component
public class ResourceManagerResourcesProvider {

    private final Map<String, ResourceProvider> resourceProviderMap;

    @Autowired
    public ResourceManagerResourcesProvider(Collection<ResourceProvider> resourceProviders) {
        this.resourceProviderMap = resourceProviders.stream().collect(toMap(ResourceProvider::getODataId, identity()));
    }

    public boolean resourceManagerResponseAvailable(String url) {
        return resourceProviderMap.containsKey(url);
    }

    public ResourceProvider getResourceProvider(String url) {
        return resourceProviderMap.get(url);
    }

    public JsonNode getResourceManagerResponse(String url) {
        return resourceProviderMap.get(url).getResource();
    }
}

