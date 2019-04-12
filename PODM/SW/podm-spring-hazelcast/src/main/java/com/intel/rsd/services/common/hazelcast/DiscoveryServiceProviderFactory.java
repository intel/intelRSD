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

package com.intel.rsd.services.common.hazelcast;

import com.hazelcast.spi.discovery.integration.DiscoveryServiceProvider;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.cloud.client.discovery.DiscoveryClient;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class DiscoveryServiceProviderFactory {
    @Bean
    DiscoveryServiceProvider discoveryServiceProvider(SpringDiscoveryService springDiscoveryService) {
        return new SpringDiscoveryProvider(springDiscoveryService);
    }

    @Bean
    SpringDiscoveryService springDiscoveryService(DiscoveryClient discoveryClient, @Value("${spring.application.name}") String applicationName) {
        return new SpringDiscoveryService(discoveryClient, applicationName);
    }
}
