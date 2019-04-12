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

package com.intel.rsd.services.common;

import org.springframework.cloud.client.ServiceInstance;
import org.springframework.cloud.client.discovery.DiscoveryClient;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class TestDiscoveryClient implements DiscoveryClient {
    private final Map<String, List<ServiceInstance>> services = new HashMap<>();

    @Override
    public String description() {
        return "test";
    }

    @Override
    public List<ServiceInstance> getInstances(String serviceId) {
        return services.getOrDefault(serviceId, Collections.emptyList());
    }

    public void addServiceInstance(String serviceType, ServiceInstance serviceInstance) {
        services.putIfAbsent(serviceType, new ArrayList<>());
        services.get(serviceType).add(serviceInstance);
    }

    @Override
    public List<String> getServices() {
        return new ArrayList<>(services.keySet());
    }
}
