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

import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.io.Resource;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import static java.nio.charset.StandardCharsets.UTF_8;
import static java.util.stream.Collectors.toList;

@Configuration
public class JsonBodyFilterConfiguration {
    @Value("${json_body_filter_config_file:classpath:/filters}")
    private Resource filters;

    @Bean
    public JsonBodyFilter jsonBodyFilter() throws IOException {
        try (InputStream inputStream = filters.getInputStream()) {
            val reader = new BufferedReader(new InputStreamReader(inputStream, UTF_8));
            val filterPatterns = reader.lines().filter(line -> !line.isEmpty()).collect(toList());

            return new JsonBodyFilter(filterPatterns);
        }
    }
}
