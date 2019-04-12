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

import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Set;

import static java.util.stream.Collectors.toSet;

@Component
@Slf4j
public class ServiceDetectorBasedRequiredServiceFinder implements RequiredServiceFinder {
    private DetectedManagersCache detectedManagersReadableCache;

    @Autowired
    public ServiceDetectorBasedRequiredServiceFinder(DetectedManagersCache detectedManagersReadableCache) {
        this.detectedManagersReadableCache = detectedManagersReadableCache;
    }

    @Override
    public URI findServiceHost(ServiceId serviceId) {
        return detectedManagersReadableCache.findUriOfRoutableManagers(serviceId.toString());
    }

    @Override
    public Set<ServiceId> getServices() {
        return detectedManagersReadableCache.getIdsOfRoutableManagers().stream()
            .map(ServiceId::new)
            .collect(toSet());
    }

}
