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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.services.configuration.client.DiscoveryServiceWebClient;
import com.intel.rsd.nodecomposer.types.discovery.DiscoveryServiceEndpoint;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class DiscoveryServiceConfigurationTask implements Runnable {
    private final DiscoveryServiceConfigurer configurer;
    private final DiscoveryServiceWebClient discoveryServiceWebClient;

    private DiscoveryServiceEndpoint discoveryServiceEndpoint;

    @Autowired
    public DiscoveryServiceConfigurationTask(DiscoveryServiceConfigurer configurer, DiscoveryServiceWebClient discoveryServiceWebClient) {
        this.configurer = configurer;
        this.discoveryServiceWebClient = discoveryServiceWebClient;
    }

    @Override
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    public void run() {
        requiresNonNull(discoveryServiceEndpoint, "DiscoveryServiceEndpoint");

        try (WebClient webClient = discoveryServiceWebClient.createWebClient(discoveryServiceEndpoint.getEndpointUri())) {
            configurer.configureUsingWebClient(webClient);
        } catch (Exception e) {
            log.error("Execution of DiscoveryService configuration task failed.", e);
        }
    }

    void setDiscoveryServiceEndpoint(DiscoveryServiceEndpoint discoveryServiceEndpoint) {
        this.discoveryServiceEndpoint = discoveryServiceEndpoint;
    }
}
