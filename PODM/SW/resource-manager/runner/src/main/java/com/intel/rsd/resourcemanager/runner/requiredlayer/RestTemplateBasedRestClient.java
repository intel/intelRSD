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
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestTemplate;

import java.net.URI;

import static java.util.Collections.singletonList;
import static org.springframework.http.MediaType.APPLICATION_JSON;

@Slf4j
final class RestTemplateBasedRestClient implements RestClient {

    private RestTemplate restTemplate;

    RestTemplateBasedRestClient(RestTemplate restTemplate) {
        this.restTemplate = restTemplate;
    }

    @Override
    public Response call(@NonNull URI uri, @NonNull HttpMethod method, @NonNull HttpHeaders headers, JsonNode body) throws ServiceAccessException {
        log.trace("{} {}", method, uri);

        try {
            headers.setAccept(singletonList(APPLICATION_JSON));
            ResponseEntity<JsonNode> entity = restTemplate.exchange(uri, method, new HttpEntity<>(body, headers), JsonNode.class);
            return new Response(entity);
        } catch (ResourceAccessException e) {
            throw new ServiceAccessException(e.getMessage(), e);
        }
    }
}
