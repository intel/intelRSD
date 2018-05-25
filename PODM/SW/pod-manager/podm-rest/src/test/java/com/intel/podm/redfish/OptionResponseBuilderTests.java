/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.redfish;

import org.testng.annotations.Test;

import javax.ws.rs.core.Response;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static com.intel.podm.redfish.OptionsResponseBuilder.newOptionsResponseBuilder;
import static javax.ws.rs.core.HttpHeaders.ALLOW;
import static org.apache.commons.lang3.StringUtils.countMatches;
import static org.jboss.resteasy.util.HttpResponseCodes.SC_OK;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;

public class OptionResponseBuilderTests {

    @Test
    public void basicConstructorReturns200OkResponse() {
        Response response = newOptionsResponseBuilder().build();
        assertEquals(response.getStatus(), SC_OK);
    }

    @Test
    public void basicConstructorReturnsWithAllowHeader() {
        Response response = newOptionsResponseBuilder().build();
        assertNotNull(response.getHeaders().get(ALLOW));
    }

    @Test
    public void basicConstructorAlwaysContainsOptionsMethod() {
        Set<String> expectedAllowedMethods = new HashSet<String>() {
            {
                add("OPTIONS");
            }
        };
        Set<String> actualAllowedMethods = newOptionsResponseBuilder().build().getAllowedMethods();

        assertEquals(actualAllowedMethods, expectedAllowedMethods);
    }

    @Test
    public void defaultOptionsConstructorHasBasicMethods() {
        Set<String> expectedAllowedMethods = new HashSet<String>() {
            {
                add("OPTIONS");
                add("HEAD");
                add("GET");
            }
        };
        Set<String> actualAllowedMethods = newOptionsForResourceBuilder().build().getAllowedMethods();

        assertEquals(actualAllowedMethods, expectedAllowedMethods);
    }

    @Test
    public void allBasicHttpMethodsWorks() {
        Set<String> expectedAllowedMethods = new HashSet<String>() {
            {
                add("OPTIONS");
                add("HEAD");
                add("GET");
                add("DELETE");
                add("PATCH");
                add("POST");
                add("PUT");

            }
        };
        Set<String> actualAllowedMethods = newOptionsResponseBuilder()
                .addGetMethod()
                .addDeleteMethod()
                .addHeadMethod()
                .addPatchMethod()
                .addPostMethod()
                .addPutMethod()
                .build()
                .getAllowedMethods();

        assertEquals(actualAllowedMethods, expectedAllowedMethods);
    }

    @Test
    public void addingMultipleTimesSameMethodWorksAsSingleAdd() {
        final int expectedGetsCount = 1;
        String allowedMethodsLine = newOptionsResponseBuilder()
                .addGetMethod()
                .addGetMethod()
                .addGetMethod()
                .addGetMethod()
                .build()
                .getHeaderString(ALLOW);

        int foundGetsStrings = countMatches(allowedMethodsLine, "GET");
        assertEquals(foundGetsStrings, expectedGetsCount);
    }
}
