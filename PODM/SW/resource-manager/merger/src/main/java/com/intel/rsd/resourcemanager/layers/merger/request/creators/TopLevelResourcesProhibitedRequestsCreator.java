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
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.ResourceManagerResourcesProvider;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import lombok.AllArgsConstructor;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;

import java.util.Map;

import static org.springframework.http.HttpMethod.GET;

@AllArgsConstructor
@Component
public class TopLevelResourcesProhibitedRequestsCreator implements SelectableRequestCreator {

    private final ResourceManagerResourcesProvider resourceManagerResourcesProvider;

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return !GET.equals(httpMethod)
            && resourceManagerResourcesProvider.resourceManagerResponseAvailable(path)
            && !resourceManagerResourcesProvider.getResourceProvider(path).isMethodSupported(httpMethod);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return layer -> new Response[]{new Response(ResponseEntity.status(HttpStatus.METHOD_NOT_ALLOWED).build())};
    }
}
