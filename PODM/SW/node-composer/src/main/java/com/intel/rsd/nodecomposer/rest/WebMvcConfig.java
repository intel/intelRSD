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

package com.intel.rsd.nodecomposer.rest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.converter.json.Jackson2ObjectMapperBuilder;
import org.springframework.web.servlet.config.annotation.InterceptorRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

import static com.fasterxml.jackson.databind.DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES;
import static com.fasterxml.jackson.databind.MapperFeature.SORT_PROPERTIES_ALPHABETICALLY;
import static com.fasterxml.jackson.databind.PropertyNamingStrategy.UPPER_CAMEL_CASE;
import static com.fasterxml.jackson.databind.SerializationFeature.FAIL_ON_EMPTY_BEANS;
import static com.fasterxml.jackson.databind.SerializationFeature.INDENT_OUTPUT;
import static com.fasterxml.jackson.databind.SerializationFeature.WRITE_DATES_AS_TIMESTAMPS;
import static com.fasterxml.jackson.databind.SerializationFeature.WRITE_DATE_TIMESTAMPS_AS_NANOSECONDS;

@Configuration
public class WebMvcConfig implements WebMvcConfigurer {
    private final RequestValidationHandlerInterceptor requestValidationHandlerInterceptor;

    @Autowired
    public WebMvcConfig(RequestValidationHandlerInterceptor requestValidationHandlerInterceptor) {
        this.requestValidationHandlerInterceptor = requestValidationHandlerInterceptor;
    }

    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        registry.addInterceptor(requestValidationHandlerInterceptor);
    }

    @Bean
    public Jackson2ObjectMapperBuilder jacksonBuilder() {
        Jackson2ObjectMapperBuilder jackson2ObjectMapperBuilder = new Jackson2ObjectMapperBuilder();
        NorthboundObjectMapperModuleProvider mapperModuleProvider = new NorthboundObjectMapperModuleProvider();

        jackson2ObjectMapperBuilder
            .propertyNamingStrategy(UPPER_CAMEL_CASE)
            .serializersByType(mapperModuleProvider.getSerializers())
            .deserializersByType(mapperModuleProvider.getDeserializers())
            .featuresToEnable(INDENT_OUTPUT, FAIL_ON_UNKNOWN_PROPERTIES)
            .featuresToDisable(WRITE_DATES_AS_TIMESTAMPS, WRITE_DATE_TIMESTAMPS_AS_NANOSECONDS, SORT_PROPERTIES_ALPHABETICALLY, FAIL_ON_EMPTY_BEANS)
            .indentOutput(true);

        return jackson2ObjectMapperBuilder;
    }
}
