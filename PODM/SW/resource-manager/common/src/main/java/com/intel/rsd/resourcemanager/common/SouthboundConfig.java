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

package com.intel.rsd.resourcemanager.common;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpMethod;
import org.springframework.http.HttpStatus.Series;

import java.util.Collection;
import java.util.List;

import static java.util.Collections.emptyList;

@Configuration
@ConfigurationProperties("southbound-config")
@Data
public class SouthboundConfig {
    private List<String> acceptedHeaders;
    private String basicAuthToken;
    private Tracing tracing;

    @Data
    public static class TracedResponse {
        private HttpMethod method;
        private Collection<Series> statusSeries;
    }

    @Data
    public static class TracedResponses {
        private Collection<Series> statusSeries = emptyList();
        private Collection<TracedResponse> definitions = emptyList();
    }

    @Data
    public static class Tracing {
        private Collection<HttpMethod> requests = emptyList();
        private TracedResponses responses;
    }
}
