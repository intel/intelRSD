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
import lombok.SneakyThrows;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;

import java.util.Map;

import static com.google.common.collect.ObjectArrays.concat;
import static com.intel.rsd.ResourcePatterns.MANAGER_PATTERN;
import static org.springframework.http.HttpMethod.GET;

@Component
public class ManagerGetRequestCreator implements SelectableRequestCreator {

    private final DetectedManagersCache detectedManagersCache;
    private final DefaultReadOnlyRequestCreator defaultReadOnlyRequestCreator;

    @Autowired
    public ManagerGetRequestCreator(DetectedManagersCache detectedManagersCache,
                                    DefaultReadOnlyRequestCreator defaultReadOnlyRequestCreator) {

        this.detectedManagersCache = detectedManagersCache;
        this.defaultReadOnlyRequestCreator = defaultReadOnlyRequestCreator;
    }

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return GET.equals(httpMethod) && MANAGER_PATTERN.matcher(path).matches();
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> {
            val underlyingServicesResponses = defaultReadOnlyRequestCreator.invoke(path, method, headers, body, requestParams, layer);
            val serviceDetectorResponses = checkOnServiceDetector(path);
            //response about status/state from SD is more important than such message from RM, so it should be first
            //Required because merger takes first argument as main argument and merges other N-1 elements with first element and then returns it
            return defaultReadOnlyRequestCreator.process(concat(serviceDetectorResponses, underlyingServicesResponses, Response.class));
        };
    }

    @SneakyThrows
    private Response[] checkOnServiceDetector(String path) {
        if (detectedManagersCache.contains(path)) {
            return new Response[]{new Response(ResponseEntity.ok(detectedManagersCache.get(path)))};
        }
        return new Response[0];
    }
}
