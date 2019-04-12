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

package com.intel.rsd.crawler;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.crawler.matcher.AllowAllResourcesUriMatcher;
import com.intel.rsd.crawler.matcher.ResourceUriMatcher;
import lombok.NonNull;
import lombok.SneakyThrows;

import java.util.ArrayDeque;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import java.util.function.Function;

import static com.google.common.collect.Sets.difference;
import static com.google.common.collect.Sets.newConcurrentHashSet;
import static java.util.concurrent.CompletableFuture.completedFuture;
import static java.util.stream.Collectors.toSet;
import static org.springframework.util.StringUtils.trimTrailingCharacter;

public class Crawler {
    private final Set<String> visited = newConcurrentHashSet();
    private final Set<Resource> result = newConcurrentHashSet();
    private final Function<String, CompletableFuture<Resource>> getter;
    private final ResourceUriMatcher resourceUriMatcher;

    public Crawler(@NonNull Function<String, CompletableFuture<Resource>> getter) {
        this(getter, new AllowAllResourcesUriMatcher());
    }

    public Crawler(@NonNull Function<String, CompletableFuture<Resource>> getter, @NonNull ResourceUriMatcher resourceUriMatcher) {
        this.getter = getter;
        this.resourceUriMatcher = resourceUriMatcher;
    }

    @SneakyThrows
    public Set<Resource> crawl() {
        crawlFrom("/redfish/v1").get();
        return result;
    }

    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:AnonInnerLength"})
    public Iterator<Resource> iterator(String rootUri) {
        return new Iterator<Resource>() {
            private final Queue<String> toBeVisitedQueue = new ArrayDeque<>();
            private String nextToVisit = rootUri;

            @Override
            public boolean hasNext() {
                return nextToVisit != null;
            }

            @Override
            public Resource next() {
                Resource resource = fetchResourceOrThrow(nextToVisit);

                Set<String> links = extractLinks(resource);

                visited.add(nextToVisit);
                toBeVisitedQueue.addAll(difference(links, visited));
                nextToVisit = toBeVisitedQueue.poll();

                result.add(resource);
                return resource;
            }

            private Resource fetchResourceOrThrow(String toVisit) {
                if (toVisit == null) {
                    throw new NoSuchElementException();
                }

                Resource resource = retrieveResource(toVisit);
                if (resource == null) {
                    throw new RuntimeException("Failed to fetch resource " + this.nextToVisit);
                }
                return resource;
            }
        };
    }

    @SneakyThrows
    private Resource retrieveResource(String odataId) {
        return getter.apply(odataId).get();
    }

    private CompletableFuture<Void> crawlFrom(String odataId) {
        if (!visited.add(odataId)) {
            return completedFuture(null);
        }

        return getter
            .apply(odataId)
            .thenApply(this::handleResource)
            .thenCompose(CompletableFuture::allOf);
    }

    private CompletableFuture[] handleResource(Resource resource) {
        if (resource == null) {
            return new CompletableFuture[0];
        }

        result.add(resource);

        Set<String> links = extractLinks(resource);
        return difference(links, visited)
            .stream()
            .map(this::crawlFrom)
            .toArray(CompletableFuture[]::new);
    }

    private Set<String> extractLinks(@NonNull Resource resource) {
        return resource.getByPath("@odata.id").stream()
            .filter(n -> !n.isNull())
            .map(JsonNode::asText)
            .filter(this::isNotJsonPointer)
            .filter(this::resourceUriMatches)
            .map(uriAsString -> trimTrailingCharacter(uriAsString, '/'))
            .collect(toSet());
    }

    private boolean isNotJsonPointer(String text) {
        return !text.contains("#");
    }

    private boolean resourceUriMatches(String uriAsString) {
        return resourceUriMatcher.uriMatches(uriAsString);
    }
}
