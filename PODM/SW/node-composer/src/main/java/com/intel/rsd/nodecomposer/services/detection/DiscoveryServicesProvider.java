/*
 * Copyright (c) 2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.discovery.external.ServiceDescriptor;
import com.intel.rsd.nodecomposer.discovery.external.UnrecognizedServiceException;
import com.intel.rsd.nodecomposer.types.discovery.DiscoveryServiceEndpoint;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Objects;
import java.util.UUID;

import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;

@Slf4j
@Component
public class DiscoveryServicesProvider {
    private final DiscoveryServiceCandidateProvider candidateProvider;
    private final ServiceDescriptor serviceDescriptor;

    @Autowired
    public DiscoveryServicesProvider(DiscoveryServiceCandidateProvider candidateProvider, ServiceDescriptor serviceDescriptor) {
        this.candidateProvider = candidateProvider;
        this.serviceDescriptor = serviceDescriptor;
    }

    Map<UUID, DiscoveryServiceEndpoint> provide() {
        return candidateProvider.getServiceCandidates()
            .map(this::asServiceEndpoint)
            .filter(Objects::nonNull)
            .collect(toMap(DiscoveryServiceEndpoint::getServiceUuid, identity()));
    }

    private DiscoveryServiceEndpoint asServiceEndpoint(DiscoveryServiceCandidate discoveryServiceCandidate) {
        try {
            return serviceDescriptor.describe(discoveryServiceCandidate.getEndpointUri());
        } catch (UnrecognizedServiceException e) {
            log.warn(e.getMessage());
            return null;
        }
    }
}
