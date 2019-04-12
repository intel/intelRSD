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

package com.intel.rsd.resourcemanager.layers.filter;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.Getter;
import lombok.val;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.List;
import java.util.Objects;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static org.assertj.core.api.Assertions.assertThat;

public class JsonBodyFilterTest {
    @Test(dataProvider = "testCases")
    public void test(List<String> patterns, String uri, ObjectNode input, ObjectNode expectedOutput) {
        val sut = new JsonBodyFilter(patterns);
        val actualOutput = sut.filterBody(uri, input);

        assertThat(actualOutput).isEqualTo(expectedOutput);
    }

    @DataProvider
    public Object[][] testCases() throws IOException {
        Data data = readJsonFromFile(this, "JsonBodyFilterTest.json", Data.class);

        return data
            .testCases
            .stream()
            .map(tc -> new Object[]{tc.filters, tc.uri, data.getResource(tc.uri), tc.expected})
            .toArray(Object[][]::new);
    }

    @Getter
    private static final class Data {
        List<Resource> resources;
        List<TestCase> testCases;

        public Object getResource(String uri) {
            return resources
                .stream()
                .filter(r -> Objects.equals(r.uri, uri))
                .map(r -> r.body)
                .findFirst()
                .orElseThrow(() -> new RuntimeException(String.format("%s not found", uri)));
        }
    }

    @Getter
    private static final class TestCase {
        String uri;
        List<String> filters;
        JsonNode expected;
    }

    @Getter
    private static final class Resource {
        String uri;
        JsonNode body;
    }
}
