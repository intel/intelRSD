/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.rest;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static org.testng.Assert.assertEquals;

public class SensitiveParametersRequestFilterTest {
    @Test(dataProvider = "requestAndResponseCollection")
    public void testFilterSecretPropertiesFromRequestWithoutSensitiveData(String request, String expectedResponse) throws Exception {
        SensitiveParametersRequestFilter filter = new SensitiveParametersRequestFilter();

        String jsonAfterFiltration = filter.filterSecretPropertiesFromRequest(request);
        assertEquals(jsonAfterFiltration, expectedResponse);
    }

    @DataProvider
    @SuppressWarnings({"checkstyle:MethodLength"})
    public Object[][] requestAndResponseCollection() {
        return new Object[][]{
            {
                " { \"test\" : \"value\",  \"param2\" : \"value2\"  }",
                " { \"test\" : \"value\",  \"param2\" : \"value2\"  }"
            },
            {
                " { \"CHAPSecret\" : \"value\",  \"param2\" : \"value2\"  }",
                "{\n  \"CHAPSecret\" : \"\",\n  \"param2\" : \"value2\"\n}"
            },
            {
                " {\"param1\" : \"value1\", \"testObject\" : { \"CHAPSecret\" : \"value\"},  \"param2\" : \"value2\"}",
                "{\n  \"param1\" : \"value1\",\n  \"testObject\" : {\n    \"CHAPSecret\" : \"\"\n  },\n  \"param2\" : \"value2\"\n}"
            },
            {
                " {\"param1\" : \"value1\", \"memebers\" : [ { \"CHAPSecret\" : \"value\"} ],  \"param2\" : \"value2\"}",
                "{\n  \"param1\" : \"value1\",\n  \"memebers\" : [ {\n    \"CHAPSecret\" : \"\"\n  } ],\n  \"param2\" : \"value2\"\n}"
            },
            {
                " { \"CHAPSecret\" : \"value\"  \"param2\" : \"value2\"  }",
                "{ }"
            },
            {
                " { \"CHAPSecret\" : \"null\",  \"param2\" : \"value2\"  }",
                "{\n  \"CHAPSecret\" : \"\",\n  \"param2\" : \"value2\"\n}"
            },
            {
                " { \"CHAPSecret\" : null,  \"param2\" : \"value2\"  }",
                "{\n  \"CHAPSecret\" : \"\",\n  \"param2\" : \"value2\"\n}"
            },
            {
                "",
                ""
            }
        };
    }
}
