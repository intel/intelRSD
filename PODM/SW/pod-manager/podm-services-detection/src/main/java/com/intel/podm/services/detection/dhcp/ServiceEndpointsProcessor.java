/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.services.detection.dhcp;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.Dhcp;
import com.intel.podm.discovery.external.ServiceEndpoint;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Predicate;

import static com.google.common.collect.Sets.difference;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
class ServiceEndpointsProcessor {

    private final ConcurrentMap<URI, ServiceEndpoint> knownServiceEndpointsMap = new ConcurrentHashMap<>();
    private final ConcurrentMap<URI, DhcpServiceCandidate> failedServiceEndpointsMap = new ConcurrentHashMap<>();

    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configHolder;

    @Inject
    private Logger logger;

    public Set<URI> getKnownUrisNotPresentInFreshCandidateSet(Set<DhcpServiceCandidate> freshCandidateSet) {
        Set<URI> knownUris = knownServiceEndpointsMap.keySet();
        Set<URI> freshCandidatesUris = freshCandidateSet.stream()
            .map(DhcpServiceCandidate::getEndpointUri)
            .collect(toSet());

        return difference(knownUris, freshCandidatesUris);
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
            && Objects.equals(knownService.getServiceType(), serviceEndpoint.getServiceType());
    }

    public void removeKnownService(URI endpointUri) {
        knownServiceEndpointsMap.remove(endpointUri);
    }

    public void removeCandidateFromFailedServices(DhcpServiceCandidate candidate) {
        failedServiceEndpointsMap.remove(candidate.getEndpointUri());
    }

    public void removeFailedServicesNotPresentInFreshCandidateSet(Set<DhcpServiceCandidate> candidates) {
        failedServiceEndpointsMap.values().removeIf(failedLocation -> !candidates.contains(failedLocation));
    }

    public void removeUpdatedCandidatesFromFailedServices(Set<DhcpServiceCandidate> candidates) {
        Set<DhcpServiceCandidate> updatedCandidates = candidates.stream()
            .filter(this::hasCandidateBeenUpdated)
            .peek(updatedCandidate -> logger.t("Service {} has been updated and is scheduled for recheck", updatedCandidate))
            .collect(toSet());
        failedServiceEndpointsMap.values().removeAll(updatedCandidates);
    }

    private boolean hasCandidateBeenUpdated(DhcpServiceCandidate service) {
        DhcpServiceCandidate failedCandidate = failedServiceEndpointsMap.get(service.getEndpointUri());

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

    public List<DhcpServiceCandidate> getCandidatesForRetry() {
        Dhcp dhcpConfig = configHolder.get().getProtocols().getDhcp();
        return failedServiceEndpointsMap.values().stream()
            .filter(service -> service.getRetries() < dhcpConfig.getRetriesForFailedServiceCheck())
            .collect(toList());
    }

    public List<DhcpServiceCandidate> getFreshCandidates(Set<DhcpServiceCandidate> candidates) {
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
        Dhcp dhcpConfig = configHolder.get().getProtocols().getDhcp();
        failedServiceEndpointsMap.values().stream()
            .filter(service -> service.getRetries() >= dhcpConfig.getRetriesForFailedServiceCheck())
            .forEach(service -> {
                knownServiceEndpointsMap.remove(service.getEndpointUri());
                failedServiceEndpointsMap.remove(service.getEndpointUri());
                logger.i("Service {} scheduled for re-check", service);
            });
    }

    /**
     * Method puts a Service Endpoint Candidate in failedServiceEndpointsMap
     * or increments its retry counter if it failed before.
     *
     * @param candidate
     */
    public void failServiceEndpointCandidate(DhcpServiceCandidate candidate) {
        DhcpServiceCandidate addedCandidate = failedServiceEndpointsMap.putIfAbsent(candidate.getEndpointUri(), candidate);
        if (addedCandidate != null) {
            addedCandidate.increaseRetries();
            logger.e("Valid service could not be detected at {}, retry no {}", addedCandidate, addedCandidate.getRetries());
        } else {
            logger.e("Valid service could not be detected at {}", candidate);
        }
    }
}
