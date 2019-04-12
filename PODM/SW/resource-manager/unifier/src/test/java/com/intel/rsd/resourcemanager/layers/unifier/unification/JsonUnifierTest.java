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

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.Getter;
import lombok.ToString;
import lombok.val;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.List;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Optional.of;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class JsonUnifierTest {
    @Mock
    UnifiedOdataIdResolver resolver;

    @BeforeMethod
    void init() {
        initMocks(this);
    }

    @Test(dataProvider = "unify")
    public void unify(List<UnificationMapping> mappings, ObjectNode input, JsonNode expected) {
        mappings.forEach(mapping -> when(resolver.toUnified(mapping.getFrom())).thenReturn(of(mapping.getTo())));
        val sut = new JsonUnifier();
        Object actual = sut.unify(input, resolver);
        assertEquals(actual, expected);
    }

    @Test(dataProvider = "deunify")
    public void deunify(List<UnificationMapping> mappings, ObjectNode input, JsonNode expected) {
        mappings.forEach(mapping -> when(resolver.toServiceSpecific(mapping.getTo())).thenReturn(of(mapping.getFrom())));
        val sut = new JsonUnifier();
        Object actual = sut.deunify(input, resolver);
        assertEquals(actual, expected);
    }

    @DataProvider(name = "unify")
    public Object[][] unifyTestCases() throws IOException {
        Data data = readJsonFromFile(this, "JsonUnifierTest.json", Data.class);
        return data
            .getTestCases()
            .stream()
            .map(tc -> new Object[]{data.getMappings(), tc.getInput(), tc.getOutput()})
            .toArray(Object[][]::new);
    }

    @DataProvider(name = "deunify")
    public Object[][] deunify() throws IOException {
        Data data = readJsonFromFile(this, "JsonUnifierTest.json", Data.class);
        return data
            .getTestCases()
            .stream()
            .filter(tc -> !tc.getOutput().has("Id"))
            .map(tc -> new Object[]{data.getMappings(), tc.getOutput(), tc.getInput()})
            .toArray(Object[][]::new);
    }

    @Getter
    private static final class Data {
        private List<UnificationMapping> mappings;
        private List<InputOutput> testCases;
    }

    @Getter
    private static final class InputOutput {
        private ObjectNode input;
        private ObjectNode output;
    }

    @Getter
    @ToString
    private static final class UnificationMapping {
        private String from;
        private String to;
    }
}
