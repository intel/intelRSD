/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Optional;

import static com.google.common.collect.ObjectArrays.concat;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.ResponseEntity.ok;

@Component
public class ManagersCollectionGetRequestCreator implements SelectableRequestCreator {

    private static final String MANAGERS_URI = "/redfish/v1/Managers";

    private final DetectedManagersCache detectedManagersCache;

    private final DefaultReadOnlyRequestCreator defaultReadOnlyRequestCreator;

    @Autowired
    public ManagersCollectionGetRequestCreator(DetectedManagersCache detectedManagersCache,
                                               DefaultReadOnlyRequestCreator defaultReadOnlyRequestCreator) {

        this.detectedManagersCache = detectedManagersCache;
        this.defaultReadOnlyRequestCreator = defaultReadOnlyRequestCreator;
    }

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return GET.equals(httpMethod) && MANAGERS_URI.equalsIgnoreCase(path);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> defaultReadOnlyRequestCreator.process(
            concat(
                defaultReadOnlyRequestCreator.invoke(path, method, headers, body, requestParams, layer),
                readServiceDetectorManagers(),
                Response.class
            )
        );
    }

    private Response[] readServiceDetectorManagers() {
        return Optional.ofNullable(detectedManagersCache.getCollection())
            .map(flatManagersCollection -> new Response[]{new Response(ok(flatManagersCollection))})
            .orElse(new Response[0]);
    }
}
