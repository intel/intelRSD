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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import org.springframework.core.annotation.Order;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.Map;

import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static org.springframework.core.Ordered.HIGHEST_PRECEDENCE;
import static org.springframework.core.Ordered.LOWEST_PRECEDENCE;
import static org.springframework.http.HttpMethod.GET;
import static org.testng.Assert.assertEquals;

public class RequestCreatorProviderTest {

    RequestCreatorProvider sut;

    @Test(expectedExceptions = UnsupportedOperationException.class)
    public void whenZeroSelectableRequestCreatorsAreRegistered_exceptionShouldBeThrown() {
        sut = new RequestCreatorProvider(emptyList());
        sut.selectCreator("/", GET);
    }

    @Test
    public void whenOneMatchingSelectableRequestCreatorIsRegistered_itShouldBeReturned() {
        First firstSelectableRequestCreator = new First();
        sut = new RequestCreatorProvider(singletonList(firstSelectableRequestCreator));
        RequestCreator result = sut.selectCreator("/", GET);
        assertEquals(result, firstSelectableRequestCreator);
    }

    @Test
    public void whenTwoMatchingSelectableRequestCreatorIsRegistered_creatorWithLowestPrecedenceShouldBeReturned() {
        First firstSelectableRequestCreator = new First();
        Second secondSelectableRequestCreator = new Second();
        sut = new RequestCreatorProvider(Arrays.asList(secondSelectableRequestCreator, firstSelectableRequestCreator));
        RequestCreator result = sut.selectCreator("/", GET);
        assertEquals(result, secondSelectableRequestCreator);
    }

    @Order(HIGHEST_PRECEDENCE)
    static class First implements SelectableRequestCreator {
        @Override
        public boolean canBeSelected(HttpMethod httpMethod, String path) {
            return true;
        }

        @Override
        public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
            return null;
        }
    }

    @Order(LOWEST_PRECEDENCE)
    static class Second implements SelectableRequestCreator {
        @Override
        public boolean canBeSelected(HttpMethod httpMethod, String path) {
            return true;
        }

        @Override
        public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
            return null;
        }
    }
}
