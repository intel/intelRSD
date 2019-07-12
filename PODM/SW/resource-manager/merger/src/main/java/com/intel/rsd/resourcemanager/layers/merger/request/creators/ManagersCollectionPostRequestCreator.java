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
import com.intel.rsd.resourcemanager.common.CacheUpdater;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.common.ServiceDetectorApiConsumer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;

import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.ResponseEntity.status;

@Component
@Slf4j
public class ManagersCollectionPostRequestCreator implements SelectableRequestCreator {

    private static final String MANAGERS_URI = "/redfish/v1/Managers";

    private final ServiceDetectorApiConsumer serviceDetectorApiConsumer;

    private final DetectedManagersCache detectedManagersCache;

    private final CacheUpdater cacheUpdater;

    @Autowired
    public ManagersCollectionPostRequestCreator(ServiceDetectorApiConsumer serviceDetectorApiConsumer,
                                                DetectedManagersCache detectedManagersCache,
                                                CacheUpdater cacheUpdater) {

        this.serviceDetectorApiConsumer = serviceDetectorApiConsumer;
        this.detectedManagersCache = detectedManagersCache;
        this.cacheUpdater = cacheUpdater;
    }

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return POST.equals(httpMethod) && MANAGERS_URI.equalsIgnoreCase(path);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> {
            val responseEntity = serviceDetectorApiConsumer.registerManager(body);
            if (responseEntity.getStatusCode().is2xxSuccessful()) {
                detectedManagersCache.update(cacheUpdater);
                return new Response[]{new Response(status(CREATED).location(responseEntity.getHeaders().getLocation()).build())};
            }

            return new Response[]{new Response(responseEntity.getBody(), responseEntity.getStatusCode())};
        };
    }
}
