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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import org.jetbrains.annotations.NotNull;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.AbstractMap.SimpleEntry;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.ResourcePatterns.COMPUTER_SYSTEM_PATTERN;
import static com.intel.rsd.collections.IterableHelper.single;
import static java.util.stream.Collectors.toList;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;

/**
 * This class provides support for RW operations for computer system.
 * It covers multiple scenarios related with having few instances of computer system represented by same UUID.
 * Received request is dispatched only to physical systems (SystemType=Physical).
 * In case when only one system is available and it is not physical METHOD_NOT_ALLOWED response is returned.
 */
@Component
public class ComputerSystemReadWriteRequestCreator implements SelectableRequestCreator {

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> execute(path, method, headers, body, requestParams, layer);
    }

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return RW_METHODS.contains(httpMethod) && COMPUTER_SYSTEM_PATTERN.matcher(path).matches();
    }

    private Response[] execute(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams,
                               Layer layer) {
        Collection<ServiceId> servicesExposingPath = layer.getServicesExposingPath(path);

        List<SimpleEntry<ServiceId, String>> targetedSystemTypeByServiceId = servicesExposingPath.stream()
            .map(serviceId -> new SimpleEntry<>(serviceId, getSystemType(layer, path, serviceId)))
            .collect(toList());

        if (targetedSystemTypeByServiceId.size() == 1 && !isPhysical(single(targetedSystemTypeByServiceId).getValue())) {
            return generateMethodNotAllowedResponse();
        } else {
            return targetedSystemTypeByServiceId.stream()
                .filter(entry -> isPhysical(entry.getValue()))
                .map(SimpleEntry::getKey)
                .map(id -> layer.invoke(id, path, method, headers, body, requestParams))
                .toArray(Response[]::new);
        }
    }

    private boolean isPhysical(String systemType) {
        return "Physical".equals(systemType);
    }


    @NotNull
    private Response[] generateMethodNotAllowedResponse() {
        return new Response[]{
            new Response(null, new HttpHeaders(), METHOD_NOT_ALLOWED)
        };
    }

    private String getSystemType(Layer layer, String path, ServiceId serviceId) {
        Response response = layer.invoke(serviceId, path, GET);
        return Optional.ofNullable(response.getBody())
            .map(body -> body.at("/SystemType").asText(null))
            .orElse("Unknown");
    }
}
