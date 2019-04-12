/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.service.detector.file;

import com.google.common.collect.Sets;
import com.intel.rsd.service.detector.ServiceEndpoint;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Predicate;

import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Slf4j
@Component
class ServiceEndpointsProcessor {

    private final ConcurrentMap<URI, ServiceEndpoint> knownServiceEndpointsMap = new ConcurrentHashMap<>();
    private final ConcurrentMap<URI, ServiceCandidate> failedServiceEndpointsMap = new ConcurrentHashMap<>();

    @Value("${NumberOfRetriesForFailedServiceCheck}")
    private long numberOfRetriesForFailedServiceCheck;

    public Set<URI> getKnownUrisNotPresentInFreshCandidateSet(Set<ServiceCandidate> freshCandidateSet) {
        Set<URI> knownUris = knownServiceEndpointsMap.keySet();
        Set<URI> freshCandidatesUris = freshCandidateSet.stream()
                .map(ServiceCandidate::getEndpointUri)
                .collect(toSet());

        return Sets.difference(knownUris, freshCandidatesUris);
    }

    public ServiceEndpoint getKnownServiceByUri(URI uri) {
        return knownServiceEndpointsMap.get(uri);
    }

    public void addKnownEndpointService(ServiceEndpoint serviceEndpoint) {
        knownServiceEndpointsMap.values().removeIf(hasMatchingEndpoint(serviceEndpoint));
        knownServiceEndpointsMap.putIfAbsent(serviceEndpoint.getEndpointUri(), serviceEndpoint);
    }

    private Predicate<ServiceEndpoint> hasMatchingEndpoint(ServiceEndpoint serviceEndpoint) {
        return knownService -> Objects.equals(knownService.getServiceUuid(), serviceEndpoint.getServiceUuid());
    }

    public void removeKnownService(URI endpointUri) {
        knownServiceEndpointsMap.remove(endpointUri);
    }

    public void removeCandidateFromFailedServices(ServiceCandidate candidate) {
        failedServiceEndpointsMap.remove(candidate.getEndpointUri());
    }

    public void removeFailedServicesNotPresentInFreshCandidateSet(Set<ServiceCandidate> candidates) {
        failedServiceEndpointsMap.values().removeIf(failedLocation -> !candidates.contains(failedLocation));
    }

    public void removeUpdatedCandidatesFromFailedServices(Set<ServiceCandidate> candidates) {
        Set<ServiceCandidate> updatedCandidates = candidates.stream()
                .filter(this::hasCandidateBeenUpdated)
                .peek(updatedCandidate -> log.trace("Service {} has been updated and is scheduled for recheck", updatedCandidate))
                .collect(toSet());
        failedServiceEndpointsMap.values().removeAll(updatedCandidates);
    }

    private boolean hasCandidateBeenUpdated(ServiceCandidate service) {
        ServiceCandidate failedCandidate = failedServiceEndpointsMap.get(service.getEndpointUri());

        if (isNull(failedCandidate)) {
            return false;
        }

        LocalDateTime failedCandidateUpdateDate = failedCandidate.getUpdateDate();
        LocalDateTime updatedCandidateUpdateDate = service.getUpdateDate();

        if (isNull(failedCandidateUpdateDate) || isNull(updatedCandidateUpdateDate)) {
            return false;
        }

        return updatedCandidateUpdateDate.isAfter(failedCandidateUpdateDate);
    }

    public List<ServiceCandidate> getCandidatesForRetry() {
        return failedServiceEndpointsMap.values().stream()
                .filter(service -> service.getRetries() < numberOfRetriesForFailedServiceCheck)
                .collect(toList());
    }

    public List<ServiceCandidate> getFreshCandidates(Set<ServiceCandidate> candidates) {
        return candidates.stream()
                .filter(service -> !knownServiceEndpointsMap.containsKey(service.getEndpointUri()))
                .filter(service -> !failedServiceEndpointsMap.containsKey(service.getEndpointUri()))
                .collect(toList());
    }

    /**
     * Method iterates through all failed services that were retried MAX_FAILED_RETRIES times
     * and schedules them for re-check. This enables detection of services that take a long time to start.
     */
    public void updateServicesListForReCheck() {
        failedServiceEndpointsMap.values().stream()
                .filter(service -> service.getRetries() >= numberOfRetriesForFailedServiceCheck)
                .forEach(service -> {
                    knownServiceEndpointsMap.remove(service.getEndpointUri());
                    failedServiceEndpointsMap.remove(service.getEndpointUri());
                    log.info("Service {} scheduled for re-check", service);
                });
    }

    /**
     * Method puts a Service Endpoint Candidate in failedServiceEndpointsMap
     * or increments its retry counter if it failed before.
     *
     * @param candidate
     */
    public void failServiceEndpointCandidate(ServiceCandidate candidate) {
        ServiceCandidate addedCandidate = failedServiceEndpointsMap.putIfAbsent(candidate.getEndpointUri(), candidate);
        if (addedCandidate != null) {
            addedCandidate.increaseRetries();
            log.error("Valid service could not be detected at {}, retry no {}", addedCandidate, addedCandidate.getRetries());
        } else {
            log.error("Valid service could not be detected at {}", candidate);
        }
    }
}
