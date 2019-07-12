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

package com.intel.rsd.resourcemanager.layers.adapter;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

@Component
@Slf4j
public class Adapter extends Layer {

    private final Collection<ResourceAdapter> resourceAdapters;

    @Autowired
    public Adapter(Collection<ResourceAdapter> resourceAdapters) {
        this.resourceAdapters = resourceAdapters;
    }

    @Override
    public Set<ServiceId> getServices() {
        return nextLayer.getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {
        val response = passToNextLayer(serviceId, path, method, headers, body, requestParams);
        val responseBody = (ObjectNode) response.getBody();

        if (responseBody == null) {
            return response;
        }

        resourceAdapters.stream()
            .filter(resourceAdapter -> resourceAdapter.shouldAdaptResource(responseBody))
            .forEach(resourceAdapter -> resourceAdapter.adapt(serviceId, responseBody));

        return response;
    }
}
