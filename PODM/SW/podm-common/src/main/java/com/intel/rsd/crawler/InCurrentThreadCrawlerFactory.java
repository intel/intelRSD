/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.crawler;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.crawler.matcher.ResourceUriMatcher;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

import java.net.URI;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;
import java.util.function.Function;

import static java.util.Collections.singletonList;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.MediaType.APPLICATION_JSON;

@Slf4j
@Component
public class InCurrentThreadCrawlerFactory {
    private RestTemplate restTemplate = new RestTemplate();

    public Crawler create(URI serviceUri) {
        return new Crawler(getInCurrentThreadResourceGetter(serviceUri));
    }

    public Crawler create(URI serviceUri, ResourceUriMatcher resourceUriMatcher) {
        return new Crawler(getInCurrentThreadResourceGetter(serviceUri), resourceUriMatcher);
    }

    private Function<String, CompletableFuture<Resource>> getInCurrentThreadResourceGetter(URI serviceUri) {
        return odataId -> CompletableFuture.completedFuture(null)
            .thenApply(ignored -> invokeGet(serviceUri, odataId))
            .handleAsync((response, ex) -> {
                if (ex != null) {
                    return null;
                }
                if (!response.getStatusCode().is2xxSuccessful()) {
                    return null;
                }
                return new SerializableResource(response.getBody());
            }, (Executor) Runnable::run);
    }

    private ResponseEntity<JsonNode> invokeGet(URI serviceUri, String odataId) {
        val uri = serviceUri.resolve(odataId);
        return restTemplate.exchange(uri, GET, new HttpEntity<>(null, createDefaultHeaders()), JsonNode.class);
    }

    private HttpHeaders createDefaultHeaders() {
        val headers = new HttpHeaders();
        headers.setAccept(singletonList(APPLICATION_JSON));
        return headers;
    }
}
