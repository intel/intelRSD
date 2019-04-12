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

import com.hazelcast.nio.Address;
import com.hazelcast.spi.discovery.DiscoveryNode;
import com.hazelcast.spi.discovery.SimpleDiscoveryNode;
import com.hazelcast.spi.discovery.integration.DiscoveryService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.cloud.client.discovery.DiscoveryClient;
import org.springframework.stereotype.Component;

import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

import static java.lang.Integer.parseInt;
import static java.util.stream.Collectors.toList;

@Slf4j
@Component
public class SpringDiscoveryService implements DiscoveryService {
    private final DiscoveryClient discoveryClient;
    private final String applicationName;

    @Autowired
    public SpringDiscoveryService(DiscoveryClient discoveryClient, @Value("${spring.application.name}") String applicationName) {
        this.discoveryClient = discoveryClient;
        this.applicationName = applicationName;
    }

    @Override
    public void start() {
    }

    @Override
    public Iterable<DiscoveryNode> discoverNodes() {
        log.trace("Discovering nodes (applicationName: {}):", applicationName);

        return discoveryClient.getInstances(applicationName).stream()
                .map(serviceInstance -> {
                    try {
                        String hazelcastHost = serviceInstance.getMetadata().get("HAZELCAST_HOST");
                        Integer hazelcastPort = parseInt(serviceInstance.getMetadata().get("HAZELCAST_PORT"));
                        Address privateAddress = new Address(hazelcastHost, hazelcastPort);
                        log.trace("Found service: {}:{}", hazelcastHost, hazelcastPort);
                        return new SimpleDiscoveryNode(privateAddress);
                    } catch (UnknownHostException | NumberFormatException e) {
                        log.warn("Could not find host: {}, {}", serviceInstance.getHost(), e);
                        return null;
                    }
                })
                .filter(Objects::nonNull)
                .collect(toList());
    }

    @Override
    public void destroy() {
    }

    @Override
    public Map<String, Object> discoverLocalMetadata() {
        return new HashMap<>();
    }
}
