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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Crawler;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.crawler.SerializableResource;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPatternsConfiguration;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.Transformers;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import lombok.Getter;
import lombok.val;
import org.springframework.core.io.ClassPathResource;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class LoadingResolverTest {
    @Test
    public void whenSourceUriIsResolvableByUnderlyingResolver_thenItShouldBeReturned() {
        val sourceUri = "/source/uri";
        val unifiedUri = "/unified/uri";

        val underlyingResolver = mock(UnifiedOdataIdResolver.class);
        when(underlyingResolver.toUnified(eq(sourceUri))).thenReturn(of(unifiedUri));

        val sut = new LoadingResolver(mock(Crawler.class), underlyingResolver);
        assertEquals(sut.toUnified(sourceUri).get(), unifiedUri);
    }

    @Test(dataProvider = "toUnifiedTestCases")
    public void testToUnified(List<Resource> knownResources, List<ObjectNode> missingResources, UnificationMapping assertion) throws IOException {
        val resource = new ClassPathResource("LoadingResolverTest.patterns", this.getClass());
        val configuration = new UnificationPatternsConfiguration(resource);
        val knownPatterns = configuration.knownPatterns();
        val unificationMappings = new UnificationMappings(new HashMap<>(), new HashMap<>(), new HashMap<>());
        val cacheBasedResolver = new CacheBasedResolver(unificationMappings, knownPatterns, mock(Transformers.class));

        val nextLayerCrawler = mock(Crawler.class);
        val resourceIterator = missingResources.stream().map(SerializableResource::new).map(Resource.class::cast).iterator();

        when(nextLayerCrawler.iterator(any())).thenReturn(resourceIterator);

        val sut = new LoadingResolver(nextLayerCrawler, cacheBasedResolver);
        sut.updateMapping(knownResources);

        assertEquals(sut.toUnified(assertion.from).get(), assertion.to);
    }

    @DataProvider(name = "toUnifiedTestCases")
    public Object[][] toUnifiedTestCases() throws IOException {
        TestData data = readJsonFromFile(this, "LoadingResolverTest.json", TestData.class);
        return data
            .getTestCases()
            .stream()
            .flatMap(tc -> tc.getAssertions().stream().map(a -> new Object[]{tc.getKnownResources(), tc.getMissingResources(), a}))
            .toArray(Object[][]::new);
    }

    @Getter
    private static final class TestData {
        private List<TestCase> testCases;
    }

    private static final class TestCase {
        @Getter
        private List<UnificationMapping> assertions;
        private List<ObjectNode> knownResources;
        private List<ObjectNode> missingResources;

        public List<Resource> getKnownResources() {
            return knownResources
                .stream()
                .map(node -> new Resource() {
                    @Override
                    public JsonNode get(String jsonPointer) {
                        return node.at(JsonPointer.compile(jsonPointer));
                    }

                    @Override
                    public List<JsonNode> getByPath(String jsonPath) {
                        return null;
                    }
                }).collect(toList());
        }

        public List<ObjectNode> getMissingResources() {
            return missingResources;
        }
    }

    @Getter
    private static final class UnificationMapping {
        private String from;
        private String to;
    }
}
