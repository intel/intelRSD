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

package com.intel.rsd.resourcemanager.runner.requiredlayer;

import com.intel.rsd.resourcemanager.common.SouthboundConfig.TracedResponse;
import com.intel.rsd.resourcemanager.common.SouthboundConfig.TracedResponses;
import com.intel.rsd.resourcemanager.common.SouthboundConfig.Tracing;
import com.intel.rsd.resourcemanager.runner.requiredlayer.SouthboundRequestLogger.TracedCallsInfoProvider;
import lombok.val;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus.Series;
import org.testng.Assert;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.Collection;
import java.util.function.Consumer;

import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.Series.CLIENT_ERROR;
import static org.springframework.http.HttpStatus.Series.INFORMATIONAL;
import static org.springframework.http.HttpStatus.Series.SERVER_ERROR;
import static org.testng.Assert.assertTrue;

public class TracedCallsInfoProviderTest {

    @Test(dataProvider = "isRequestTracedTestProvider")
    public void isRequestTracedTest(HttpMethod httpMethod) {
        val tracingConfiguration = createTracing(new TracedResponses(), httpMethod);

        val sut = new TracedCallsInfoProvider(tracingConfiguration);

        assertTrue(sut.isRequestTraced(httpMethod));
    }

    @SuppressWarnings("checkstyle:AnonInnerLength")
    @Test(dataProvider = "isResponseTracedTestProvider")
    public void isResponseTracedTest(HttpMethod method, Collection<Series> acceptedStatusSeries, Series returnedStatusSeries, Consumer<Boolean> assertion) {
        val tracedResponse = createTracedResponse(method, acceptedStatusSeries);
        val tracedResponses = createTracedResponses(tracedResponse);
        val tracingConfiguration = createTracing(tracedResponses);

        val sut = new TracedCallsInfoProvider(tracingConfiguration);
        assertion.accept(sut.isResponseTraced(method, returnedStatusSeries));
    }

    @DataProvider
    private Object[][] isRequestTracedTestProvider() {
        return Arrays.stream(HttpMethod.values()).map(m -> new Object[]{m}).toArray(Object[][]::new);
    }

    @DataProvider
    private Object[][] isResponseTracedTestProvider() {
        return new Object[][]{
            new Object[]{GET, asList(CLIENT_ERROR, INFORMATIONAL), CLIENT_ERROR, (Consumer<Boolean>) Assert::assertTrue},
            new Object[]{POST, asList(CLIENT_ERROR, INFORMATIONAL), CLIENT_ERROR, (Consumer<Boolean>) Assert::assertTrue},
            new Object[]{POST, asList(CLIENT_ERROR, INFORMATIONAL), SERVER_ERROR, (Consumer<Boolean>) Assert::assertFalse},
        };
    }

    private Tracing createTracing(TracedResponses tracedResponses, HttpMethod... httpMethods) {
        val tracing = new Tracing();
        tracing.setRequests(asList(httpMethods));
        tracing.setResponses(tracedResponses);
        return tracing;
    }

    private TracedResponses createTracedResponses(TracedResponse... responses) {
        val tracedResponses = new TracedResponses();
        tracedResponses.setDefinitions(asList(responses));
        tracedResponses.setStatusSeries(emptyList());
        return tracedResponses;
    }

    private TracedResponse createTracedResponse(HttpMethod httpMethod, Collection<Series> acceptedStatusSeries) {
        val response = new TracedResponse();
        response.setMethod(httpMethod);
        response.setStatusSeries(acceptedStatusSeries);
        return response;
    }
}
