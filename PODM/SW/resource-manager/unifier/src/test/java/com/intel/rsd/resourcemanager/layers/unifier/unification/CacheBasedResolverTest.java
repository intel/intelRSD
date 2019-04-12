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

import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPatternsConfiguration;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.Transformers;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import com.intel.rsd.resourcemanager.layers.unifier.unification.dtos.Data;
import lombok.val;
import org.springframework.core.io.ClassPathResource;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static org.mockito.Mockito.mock;
import static org.testng.Assert.assertEquals;

public class CacheBasedResolverTest {
    private CacheBasedResolver sut;

    @BeforeTest
    void beforeTest() throws IOException {
        val resource = new ClassPathResource("UnifiedOdataIdResolverImplTest.patterns", this.getClass());
        val configuration = new UnificationPatternsConfiguration(resource);
        val knownPatterns = configuration.knownPatterns();
        val unificationMappings = new UnificationMappings(new HashMap<>(), new HashMap<>(), new HashMap<>());
        sut = new CacheBasedResolver(unificationMappings, knownPatterns, mock(Transformers.class));
    }

    @Test(dataProvider = "toUnifiedTestCases")
    public void testToUnified(List<Resource> resources, String from, String expected) {
        sut.updateMapping(resources);

        assertEquals(sut.toUnified(from).get(), expected);
    }

    @Test(dataProvider = "toServiceSpecificTestCases")
    public void testToServiceSpecific(List<Resource> resources, String unifiedOdataId, String expected) {
        sut.updateMapping(resources);

        assertEquals(sut.toServiceSpecific(unifiedOdataId).get(), expected);
    }

    @DataProvider(name = "toUnifiedTestCases")
    public Object[][] toUnifiedTestCases() throws IOException {
        Data data = readJsonFromFile(this, "UnifiedOdataIdResolverImplTest.json", Data.class);
        return data
            .getTestCases()
            .stream()
            .flatMap(tc -> tc.getAssertions().stream().map(a -> new Object[]{tc.getResources(), a.getFrom(), a.getTo()}))
            .toArray(Object[][]::new);
    }

    @DataProvider(name = "toServiceSpecificTestCases")
    public Object[][] toServiceSpecificTestCases() throws IOException {
        Data data = readJsonFromFile(this, "UnifiedOdataIdResolverImplTest.json", Data.class);
        return data
            .getTestCases()
            .stream()
            .flatMap(tc -> tc.getAssertions().stream().map(a -> new Object[]{tc.getResources(), a.getTo(), a.getFrom()}))
            .toArray(Object[][]::new);
    }
}
