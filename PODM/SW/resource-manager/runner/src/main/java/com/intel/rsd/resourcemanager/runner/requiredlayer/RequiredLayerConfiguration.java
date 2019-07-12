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

import com.intel.rsd.http.HtmlErrorToJsonNodeConverter;
import com.intel.rsd.http.RsdClient;
import lombok.val;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.BufferingClientHttpRequestFactory;
import org.springframework.http.client.ClientHttpRequestFactory;
import org.springframework.http.client.ClientHttpRequestInterceptor;
import org.springframework.http.converter.HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import java.util.Collection;

@Configuration
public class RequiredLayerConfiguration {
    @Bean
    RestTemplateBasedRestClient restTemplateBasedRestClient(@RsdClient ClientHttpRequestFactory httpComponentsClientHttpRequestFactory,
                                                            Collection<HttpMessageConverter<?>> additionalMessageConverters,
                                                            @RsdClient Collection<ClientHttpRequestInterceptor> additionalInterceptors) {
        val bufferingRequestFactory = new BufferingClientHttpRequestFactory(httpComponentsClientHttpRequestFactory);
        val rsdRestTemplate = createRestTemplate(bufferingRequestFactory, additionalMessageConverters, additionalInterceptors);
        return new RestTemplateBasedRestClient(rsdRestTemplate);
    }

    @Bean
    @RsdClient
    HttpMessageConverter htmlErrorToJsonNodeConverter() {
        return new HtmlErrorToJsonNodeConverter();
    }

    private RestTemplate createRestTemplate(ClientHttpRequestFactory clientHttpRequestFactory,
                                            Collection<HttpMessageConverter<?>> additionalMessageConverters,
                                            Collection<ClientHttpRequestInterceptor> additionalInterceptors) {
        return new RsdRestTemplate(clientHttpRequestFactory, additionalMessageConverters, additionalInterceptors);
    }
}
