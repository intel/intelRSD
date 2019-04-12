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

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.matcher.AllowResourcesUsingPatternsUriMatcher;
import lombok.Getter;
import lombok.val;
import org.springframework.core.io.ClassPathResource;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;
import java.util.function.Function;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.google.common.collect.ImmutableSet.of;
import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.nio.charset.StandardCharsets.UTF_8;
import static java.nio.file.Files.readAllLines;
import static java.util.concurrent.CompletableFuture.completedFuture;
import static java.util.concurrent.CompletableFuture.supplyAsync;
import static java.util.concurrent.Executors.newFixedThreadPool;
import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;
import static java.util.stream.Collectors.toSet;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:AnonInnerLength"})
public class CrawlerTest {
    private static final Executor CONCURRENT_EXECUTOR = newFixedThreadPool(10);
    private static final Collection<String> ALLOWABLE_PATTERNS = new HashSet<>();
    private static final Collection<String> ALLOWABLE_ODATA_TYPES = new HashSet<>();

    @BeforeClass
    private void prepareAllowableData() throws IOException {
        ClassPathResource classPathResource = new ClassPathResource("allowableTestPatterns");
        ALLOWABLE_PATTERNS.addAll(readAllLines(classPathResource.getFile().toPath(), UTF_8).stream()
            .filter(pattern -> !isNullOrEmpty(pattern))
            .collect(toSet()));

        ALLOWABLE_ODATA_TYPES.addAll(
            of("#ServiceRoot.v1_1_1.ServiceRoot", "#ComputerSystemCollection.ComputerSystemCollection", "#ComputerSystem.v1_3_0.ComputerSystem",
                "#EthernetInterfaceCollection.EthernetInterfaceCollection", "#EthernetInterface.v1_1_0.EthernetInterface",
                "#StorageServiceCollection.StorageServiceCollection", "#StorageService.v1_0_0.StorageService", "#StorageDriveCollection.StorageDriveCollection",
                "#Drive.v1_2_0.Drive", "#StoragePoolCollection.StoragePoolCollection")
        );
    }

    @Test(dataProvider = "testCases")
    public void whenCrawlingWithoutConcurrencyShouldFetchAllResources(Set<Resource> expectedResources) {
        Crawler sut = new Crawler(buildGetter(expectedResources));

        Set<Resource> actualResources = sut.crawl();

        assertEquals(actualResources, expectedResources);
    }

    @Test(dataProvider = "testCases")
    public void whenIteratingShouldFetchAllResources(Set<Resource> expectedResources) {
        Crawler sut = new Crawler(buildGetter(expectedResources));

        Set<Resource> actualResources = new HashSet<>();
        sut.iterator("/redfish/v1").forEachRemaining(actualResources::add);

        assertEquals(actualResources, expectedResources);
    }

    @Test(dataProvider = "testCases", invocationCount = 10, dependsOnMethods = "whenCrawlingWithoutConcurrencyShouldFetchAllResources")
    public void whenCrawlingWithConcurrencyShouldVisitEachResourceExactlyOnce(Set<Resource> expectedResources) {
        Crawler sut = new Crawler(buildConcurrentGetter(expectedResources));

        Set<Resource> actualResources = sut.crawl();

        assertEquals(actualResources, expectedResources);
    }

    @Test(dataProvider = "testCases")
    public void whenCrawlingWithoutConcurrencyWithUriMatcherShouldFetchOnlyAllowableResources(Set<Resource> allResources) {
        Crawler sut = new Crawler(buildGetter(allResources), new AllowResourcesUsingPatternsUriMatcher(ALLOWABLE_PATTERNS));
        Set<Resource> expectedResources = filterResources(allResources);

        Set<Resource> actualResources = sut.crawl();

        assertEquals(actualResources, expectedResources);
    }

    @Test(dataProvider = "testCases")
    public void whenIteratingWithUriMatcherShouldFetchOnlyAllowableResources(Set<Resource> allResources) {
        Crawler sut = new Crawler(buildGetter(allResources), new AllowResourcesUsingPatternsUriMatcher(ALLOWABLE_PATTERNS));
        Set<Resource> expectedResources = filterResources(allResources);

        Set<Resource> actualResources = new HashSet<>();
        sut.iterator("/redfish/v1").forEachRemaining(actualResources::add);

        assertEquals(actualResources, expectedResources);
    }

    @Test(dataProvider = "testCases", invocationCount = 10, dependsOnMethods = "whenCrawlingWithoutConcurrencyWithUriMatcherShouldFetchOnlyAllowableResources")
    public void whenCrawlingWithConcurrencyWithUriMatcherShouldVisitEachAllowableResourceExactlyOnce(Set<Resource> allResources) {
        Crawler sut = new Crawler(buildConcurrentGetter(allResources), new AllowResourcesUsingPatternsUriMatcher(ALLOWABLE_PATTERNS));
        Set<Resource> expectedResources = filterResources(allResources);

        Set<Resource> actualResources = sut.crawl();

        assertEquals(actualResources, expectedResources);
    }

    private Function<String, CompletableFuture<Resource>> buildConcurrentGetter(Set<Resource> resources) {
        val map = resources.stream().collect(toMap(Resource::getOdataId, identity()));

        HashSet<String> visited = new HashSet<>();

        return odataId -> supplyAsync(() -> {
            assertTrue(visited.add(odataId));
            return map.get(odataId);
        }, CONCURRENT_EXECUTOR);
    }

    private Function<String, CompletableFuture<Resource>> buildGetter(Set<Resource> resources) {
        val map = resources.stream().collect(toMap(Resource::getOdataId, identity()));

        return odataId -> {
            val resource = map.get(odataId);
            return resource != null
                ? completedFuture(resource)
                : supplyAsync(() -> {
                throw new RuntimeException(odataId);
            });
        };
    }

    private Set<Resource> filterResources(Set<Resource> allResources) {
        return allResources.stream()
            .filter(resource -> ALLOWABLE_ODATA_TYPES.contains(resource.get("/@odata.type").asText()))
            .collect(toSet());
    }

    @DataProvider(name = "testCases")
    public Object[][] testCases() throws IOException {
        return readJsonFromFile(this, "CrawlerTest.json", Data.class)
            .getTestCases()
            .stream()
            .map(tc -> new Object[]{toResources(tc.getResources())})
            .toArray(Object[][]::new);
    }

    private Set<Resource> toResources(List<ObjectNode> resources) {
        return resources.stream().map(this::toResource).collect(toSet());
    }

    private Resource toResource(ObjectNode objectNode) {
        return new Resource() {
            @Override
            public JsonNode get(String jsonPointer) {
                return objectNode.at(JsonPointer.compile(jsonPointer));
            }

            @Override
            public List<JsonNode> getByPath(String jsonPath) {
                return objectNode.findValues(jsonPath);
            }

            @Override
            public String toString() {
                return getOdataId();
            }
        };
    }

    @Getter
    public static final class Data {
        private List<TestCase> testCases;
    }

    @Getter
    public static final class TestCase {
        private List<ObjectNode> resources;
    }
}
