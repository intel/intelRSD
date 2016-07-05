/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.services.detection;

import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.time.LocalDateTime;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Predicate;

import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
class ServiceEndpointsProcessor {

    private static final int MAX_FAILED_RETRIES = 5;
    private final ConcurrentMap<URI, ServiceEndpoint> knownServiceEndpointsMap = new ConcurrentHashMap<>();
    private final ConcurrentMap<URI, ServiceEndpointCandidate> failedServiceEndpointsMap = new ConcurrentHashMap<>();

    @Inject
    private Logger logger;

    public Set<URI> getKnownUrisNotPresentInFreshCandidateSet(Set<ServiceEndpointCandidate> freshCandidateSet) {
        Set<URI> staleCandidateSet = new HashSet<>();
        for (URI knownServiceEndpointUri : knownServiceEndpointsMap.keySet()) {
            boolean presentInFreshCandidateSet = false;
            for (ServiceEndpointCandidate candidate : freshCandidateSet) {
                if (candidate.getEndpointUri().equals(knownServiceEndpointUri)) {
                    presentInFreshCandidateSet = true;
                    break;
                }
            }
            if (!presentInFreshCandidateSet) {
                staleCandidateSet.add(knownServiceEndpointUri);
            }
        }
        return staleCandidateSet;
    }

    public ServiceEndpoint getKnownServiceByUri(URI uri) {
        return knownServiceEndpointsMap.get(uri);
    }

    public void addKnownEndpointService(ServiceEndpoint serviceEndpoint) {
        knownServiceEndpointsMap.values().removeIf(hasMatchingEndpoint(serviceEndpoint));
        knownServiceEndpointsMap.putIfAbsent(serviceEndpoint.getEndpointUri(), serviceEndpoint);
    }

    private Predicate<ServiceEndpoint> hasMatchingEndpoint(ServiceEndpoint serviceEndpoint) {
        return knownService -> Objects.equals(knownService.getServiceUuid(), serviceEndpoint.getServiceUuid())
            && Objects.equals(knownService.getMacAddress(), serviceEndpoint.getMacAddress())
            && Objects.equals(knownService.getServiceType(), serviceEndpoint.getServiceType());
    }

    public void removeKnownService(URI endpointUri) {
        knownServiceEndpointsMap.remove(endpointUri);
    }

    public void removeCandidateFromFailedServices(ServiceEndpointCandidate candidate) {
        failedServiceEndpointsMap.remove(candidate.getEndpointUri());
    }

    public void removeFailedServicesNotPresentInFreshCandidateSet(Set<ServiceEndpointCandidate> candidates) {
        failedServiceEndpointsMap.values().removeIf(failedLocation -> !candidates.contains(failedLocation));
    }

    public void removeUpdatedCandidatesFromFailedServices(Set<ServiceEndpointCandidate> candidates) {
        Set<ServiceEndpointCandidate> updatedCandidates = candidates.stream()
                .filter(this::hasCandidateBeenUpdated)
                .peek(updatedCandidate -> logger.t("Service {} has been updated and is scheduled for recheck", updatedCandidate))
                .collect(toSet());
        failedServiceEndpointsMap.values().removeAll(updatedCandidates);
    }

    private boolean hasCandidateBeenUpdated(ServiceEndpointCandidate service) {
        ServiceEndpointCandidate failedCandidate = failedServiceEndpointsMap.get(service.getEndpointUri());

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

    public List<ServiceEndpointCandidate> getCandidatesForRetry() {
        return failedServiceEndpointsMap.values().stream()
                .filter(service -> service.getRetries() < MAX_FAILED_RETRIES)
                .collect(toList());
    }

    public List<ServiceEndpointCandidate> getFreshCandidates(Set<ServiceEndpointCandidate> candidates) {
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
                .filter(service -> service.getRetries() >= MAX_FAILED_RETRIES)
                .forEach(service -> {
                    knownServiceEndpointsMap.remove(service.getEndpointUri());
                    failedServiceEndpointsMap.remove(service.getEndpointUri());
                    logger.i("Service {} scheduled for re-check", service);
                });
    }

    public ServiceEndpoint getServiceEndpointForUuid(UUID serviceUuid) {
        return knownServiceEndpointsMap.values().stream()
                .filter(se -> se.getServiceUuid().equals(serviceUuid))
                .findFirst().orElse(null);
    }

    /**
     * Method puts a Service Endpoint Candidate in failedServiceEndpointsMap
     * or increments its retry counter if it failed before.
     *
     * @param candidate
     */
    public void failServiceEndpointCandidate(ServiceEndpointCandidate candidate) {
        ServiceEndpointCandidate addedCandidate = failedServiceEndpointsMap.putIfAbsent(candidate.getEndpointUri(), candidate);
        if (addedCandidate != null) {
            addedCandidate.increaseRetries();
            logger.e("Valid service could not be detected at {}, retry no {}", addedCandidate, addedCandidate.getRetries());
        } else {
            logger.e("Valid service could not be detected at {}", candidate);
        }
    }

    public void filterKnownServices(Set<ServiceEndpoint> knownEndpoints) {
        knownServiceEndpointsMap.values().removeIf(knownLocation -> !knownEndpoints.contains(knownLocation));
    }
}
