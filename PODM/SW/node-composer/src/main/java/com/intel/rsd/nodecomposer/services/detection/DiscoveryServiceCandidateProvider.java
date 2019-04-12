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

package com.intel.rsd.nodecomposer.services.detection;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cloud.client.loadbalancer.LoadBalancerClient;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

import java.net.URI;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;

import static java.util.stream.Stream.empty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class DiscoveryServiceCandidateProvider {

    private static final TextNode DISCOVERY_SERVICE = new TextNode("DiscoveryService");

    private final LoadBalancerClient loadBalancerClient;
    private final RestTemplate restTemplate;

    @Autowired
    public DiscoveryServiceCandidateProvider(LoadBalancerClient loadBalancerClient) {
        this.loadBalancerClient = loadBalancerClient;
        this.restTemplate = new RestTemplate();
    }

    Stream<DiscoveryServiceCandidate> getServiceCandidates() {
        val serviceInstance = loadBalancerClient.choose("service-detector");
        if (serviceInstance == null) {
            return empty();
        }

        val uri = URI.create(String.format("http://%s:%s/redfish/v1/Managers?$expand=.($levels=1)", serviceInstance.getHost(), serviceInstance.getPort()));

        val expandedManagers = restTemplate.getForObject(uri, JsonNode.class);
        return StreamSupport.stream(expandedManagers.withArray("Members").spliterator(), false)
            .filter(member -> DISCOVERY_SERVICE.equals(member.get("RemoteRedfishServiceType")))
            .map(member -> member.get("RemoteRedfishServiceUri").asText())
            .map(URI::create)
            .map(DiscoveryServiceCandidate::new);
    }
}
