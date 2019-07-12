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

package com.intel.rsd.resourcemanager.runner.requiredlayer;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.collections.Lists;
import com.intel.rsd.resourcemanager.common.SouthboundConfig;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpHeaders.EMPTY;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;

@Component
@Slf4j
public class RequiredLayer extends Layer {

    private final RestClient restClient;

    private final RequiredServiceFinder finder;

    private final Collection<String> acceptedHeaders;

    private final String basicAuthToken;

    @Autowired
    RequiredLayer(RestClient restClient, RequiredServiceFinder finder, SouthboundConfig southboundConfig) {
        this.restClient = restClient;
        this.finder = finder;
        this.acceptedHeaders = southboundConfig.getAcceptedHeaders();
        this.basicAuthToken = southboundConfig.getBasicAuthToken();
    }

    @Override
    public Set<ServiceId> getServices() {
        return finder.getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {
        val serviceHost = finder.findServiceHost(serviceId);
        if (serviceHost == null) {
            return serviceNotAvailableResponse();
        }
        val uri = serviceHost.resolve(path);
        addBasicAuthHeaderIfNecessary(headers);
        try {
            Response response = restClient.call(uri, method, headers, body);
            HttpHeaders httpHeaders = response.getHttpHeaders().entrySet().stream()
                .filter(entry -> acceptedHeaders.contains(entry.getKey()))
                .collect(toMap(Map.Entry::getKey, Map.Entry::getValue, Lists::concat, HttpHeaders::new));

            return new Response(response.getBody(), httpHeaders, response.getHttpStatus());
        } catch (ServiceAccessException e) {
            log.info("Service({}) unavailable: {}", serviceId, e.getMessage());
            log.trace("Exception has been thrown during accessing URI({}) on service({})", path, serviceId, e);
            return serviceNotAvailableResponse();
        }
    }

    private Response serviceNotAvailableResponse() {
        return new Response(null, EMPTY, SERVICE_UNAVAILABLE);
    }

    private void addBasicAuthHeaderIfNecessary(@NonNull HttpHeaders headers) {
        if (basicAuthToken != null && !basicAuthToken.isEmpty()) {
            headers.add("Authorization", "Basic " + basicAuthToken);
        }
    }

}
