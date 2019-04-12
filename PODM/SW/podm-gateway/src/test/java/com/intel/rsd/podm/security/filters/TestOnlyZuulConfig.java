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

package com.intel.rsd.podm.security.filters;

import org.springframework.cloud.netflix.zuul.filters.route.FallbackProvider;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.client.ClientHttpResponse;

import java.io.InputStream;

import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.MediaType.APPLICATION_JSON;

@Configuration
public class TestOnlyZuulConfig {

    @SuppressWarnings("unused")
    @Bean
    public TestOnlyFallback nodeComposerFallbackProvider() {
        return new TestOnlyFallback("node-composer");
    }

    @SuppressWarnings("unused")
    class TestOnlyFallback implements FallbackProvider {

        private final String route;

        TestOnlyFallback(String route) {
            this.route = route;
        }

        @Override
        public String getRoute() {
            return route;
        }

        @Override
        public ClientHttpResponse fallbackResponse(String route, Throwable cause) {
            return new FallbackResponse();
        }

        private final class FallbackResponse implements ClientHttpResponse {
            @Override
            public HttpHeaders getHeaders() {
                HttpHeaders headers = new HttpHeaders();
                headers.setContentType(APPLICATION_JSON);
                return headers;
            }

            @Override
            public InputStream getBody() {
                return null;
            }

            @Override
            public HttpStatus getStatusCode() {
                return OK;
            }

            @Override
            public int getRawStatusCode() {
                return OK.value();
            }

            @Override
            public String getStatusText() {
                return "We're fine.";
            }

            @Override
            public void close() {

            }
        }
    }
}
