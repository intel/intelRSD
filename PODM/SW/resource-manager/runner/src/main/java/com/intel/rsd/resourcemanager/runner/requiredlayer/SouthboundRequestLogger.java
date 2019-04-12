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

package com.intel.rsd.resourcemanager.runner.requiredlayer;

import com.intel.rsd.http.RsdClient;
import com.intel.rsd.resourcemanager.common.SouthboundConfig;
import com.intel.rsd.resourcemanager.common.SouthboundConfig.TracedResponse;
import com.intel.rsd.resourcemanager.common.SouthboundConfig.Tracing;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpRequest;
import org.springframework.http.HttpStatus.Series;
import org.springframework.http.client.ClientHttpRequestExecution;
import org.springframework.http.client.ClientHttpRequestInterceptor;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.stereotype.Component;

import java.io.IOException;
import java.util.Collection;
import java.util.Map;

import static java.nio.charset.Charset.defaultCharset;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toMap;
import static org.springframework.util.StreamUtils.copyToString;

@Slf4j
@Component
@RsdClient
public class SouthboundRequestLogger implements ClientHttpRequestInterceptor {

    private final TracedCallsInfoProvider tracedCallsInfoProvider;

    @Autowired
    public SouthboundRequestLogger(SouthboundConfig southboundConfig) {
        this.tracedCallsInfoProvider = new TracedCallsInfoProvider(southboundConfig.getTracing());
    }

    @Override
    public ClientHttpResponse intercept(HttpRequest request, @NonNull byte[] body, ClientHttpRequestExecution execution) throws IOException {
        if (tracedCallsInfoProvider.isRequestTraced(request.getMethod())) {
            log.trace("Invoking {} with body {} on {}", request.getMethod(), new String(body), request.getURI());
        }

        val response = execution.execute(request, body);

        if (tracedCallsInfoProvider.isResponseTraced(request.getMethod(), response.getStatusCode().series())) {
            log.trace("Service({}) replied with status({}): {}",
                request.getURI(),
                response.getStatusCode(),
                copyToString(response.getBody(), defaultCharset())
            );
        }

        return response;
    }

    static class TracedCallsInfoProvider {
        private final Collection<HttpMethod> tracedRequests;
        private final Map<HttpMethod, Collection<Series>> tracedResponses;

        TracedCallsInfoProvider(Tracing tracingConfiguration) {
            tracedRequests = tracingConfiguration.getRequests();

            val defaultResponseStatusSeries = tracingConfiguration.getResponses().getStatusSeries();
            tracedResponses = tracingConfiguration.getResponses().getDefinitions().stream()
                .collect(
                    toMap(
                        TracedResponse::getMethod,
                        tr -> ofNullable(tr.getStatusSeries()).orElse(defaultResponseStatusSeries)
                    )
                );
        }

        boolean isRequestTraced(@NonNull HttpMethod httpMethod) {
            return tracedRequests.contains(httpMethod);
        }

        boolean isResponseTraced(@NonNull HttpMethod httpMethod, Series statusCodeSeries) {
            return tracedResponses.containsKey(httpMethod) && tracedResponses.get(httpMethod).contains(statusCodeSeries);
        }
    }
}
