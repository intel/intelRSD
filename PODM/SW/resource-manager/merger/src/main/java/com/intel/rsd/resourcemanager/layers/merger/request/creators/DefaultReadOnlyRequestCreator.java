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
import com.intel.rsd.resourcemanager.layers.merger.ResourceManagerResourcesProvider;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.annotation.Order;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.concurrent.ForkJoinPool;
import java.util.function.Function;
import java.util.stream.Stream;

import static org.springframework.core.Ordered.HIGHEST_PRECEDENCE;
import static org.springframework.http.ResponseEntity.ok;

@Component
@Slf4j
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
@Order(HIGHEST_PRECEDENCE)
class DefaultReadOnlyRequestCreator implements SelectableRequestCreator {

    private final ResourceManagerResourcesProvider resourceManagerResourcesProvider;

    private final ForkJoinPool customThreadPool;

    @Autowired
    DefaultReadOnlyRequestCreator(ResourceManagerResourcesProvider resourceManagerResourcesProvider,
                                  @Value("${requestcreator.thread.pool.size:20}") int requestCreatorThreadPoolSize) {

        this.resourceManagerResourcesProvider = resourceManagerResourcesProvider;
        this.customThreadPool = new ForkJoinPool(requestCreatorThreadPoolSize);
    }


    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return HttpMethod.GET.equals(httpMethod);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> {
            val responses = invoke(path, method, headers, body, requestParams, layer);
            return process(responses);
        };
    }

    protected Response[] process(Response[] responses) {
        if (responses.length > 1) { //if there is more than 1 response remove unsuccessful ones
            return Stream.of(responses)
                .filter(response -> response.getHttpStatus().is2xxSuccessful())
                .toArray(Response[]::new);
        }

        return responses;
    }

    protected Response[] invoke(String path,
                                HttpMethod method,
                                HttpHeaders headers,
                                JsonNode body,
                                Map<String, String> requestParams,
                                Layer layer) {

        return Stream.concat(
            invokeLocally(path),
            invokeRemotely(path, method, headers, body, requestParams, layer)
        ).toArray(Response[]::new);
    }

    @SneakyThrows
    private Stream<Response> invokeRemotely(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams,
                                            Layer layer) {
        val affectedServices = layer.getServicesExposingPath(path);
        val executor = selectExecutor(affectedServices);
        val responses = executor.execute(affectedServices, serviceId -> layer.invoke(serviceId, path, method, headers, body, requestParams));
        return responses.peek(response -> logNon20xResponses(path, response));
    }

    private RequestExecutor selectExecutor(Collection<ServiceId> affectedServices) {
        if (affectedServices.size() == 1) {
            return (servicesToQuery, request) -> servicesToQuery.stream().map(request);
        } else {
            return (servicesToQuery, request) -> customThreadPool.submit(() -> servicesToQuery.parallelStream().map(request)).get();
        }
    }

    private void logNon20xResponses(String path, Response response) {
        if (!response.getHttpStatus().is2xxSuccessful()) {
            log.warn("GET request({}) finished with non 20x status({}) response: {}", path, response.getHttpStatus(), response.getBody());
        }
    }

    private Stream<Response> invokeLocally(String path) {
        return resourceManagerResourcesProvider.resourceManagerResponseAvailable(path)
            ? Stream.of(new Response(ok(resourceManagerResourcesProvider.getResourceManagerResponse(path))))
            : Stream.empty();
    }

    private interface RequestExecutor {
        Stream<Response> execute(Collection<ServiceId> affectedServices, Function<ServiceId, Response> request) throws Exception;
    }
}
