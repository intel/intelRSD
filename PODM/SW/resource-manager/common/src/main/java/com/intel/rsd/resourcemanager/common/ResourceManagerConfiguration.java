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

package com.intel.rsd.resourcemanager.common;

import com.fasterxml.jackson.databind.node.NullNode;
import com.hazelcast.core.IAtomicReference;
import com.intel.rsd.services.common.Shared;
import org.springframework.cloud.client.loadbalancer.LoadBalanced;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.web.client.DefaultResponseErrorHandler;
import org.springframework.web.client.RestTemplate;

import java.net.URI;
import java.util.Map;

@Configuration
public class ResourceManagerConfiguration {

    @Bean
    @LoadBalanced
    public RestTemplate loadBalancedRestTemplate() {
        RestTemplate template = new RestTemplate();
        template.setErrorHandler(new DefaultResponseErrorHandler() {
            @Override
            public boolean hasError(ClientHttpResponse response) {
                return false;
            }
        });
        return template;
    }

    @Bean
    public ServiceDetectorApiConsumer serviceDetectorApiConsumer() {
        return new ServiceDetectorApiConsumer(loadBalancedRestTemplate());
    }

    @Bean
    public DetectedManagersCache detectedManagersCache(@Shared("rm:sd-managers:uriToBodyMapping") Map<?, ?> uriToBodyMapping,
                                                       @Shared("rm:sd-managers:routable-managers:uuidToUriMapping") Map<?, ?> uuidToUriMapping,
                                                       @Shared("rm:sd-managers:collection") IAtomicReference<String> atomicReferenceToManagerCollection) {


        if (atomicReferenceToManagerCollection.isNull()) {
            atomicReferenceToManagerCollection.set(NullNode.getInstance().asText());
        }

        return new DetectedManagersCache(
            (Map<String, String>) uriToBodyMapping,
            (Map<String, URI>) uuidToUriMapping,
            atomicReferenceToManagerCollection
        );
    }
}
