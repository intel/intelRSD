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

import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import java.net.URI;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static java.util.stream.Collectors.toSet;
import static javax.ejb.LockType.WRITE;

/**
 * Class responsible for detecting service presence and notifying
 * discovery service (providing it with service endpoints) if a valid
 * service was detected at given URI. Notifications about known,
 * discovered services are not repeated.
 */
@Singleton
@Lock(WRITE)
public class ServiceChecker {

    @Inject
    private Logger logger;

    @Inject
    private ServiceDetectionListener serviceDetectionListener;

    @Inject
    private ServiceDetection serviceDetection;

    @Inject
    private ServiceEndpointsProcessor serviceEndpointsProcessor;

    @Inject
    private ServiceEndpointCandidatesProvider serviceEndpointCandidatesProvider;

    /**
     * Method initiates service detection for all provided Service Endpoint Candidates.
     */
    public void triggerEndpointCandidatesCheck() {
        Set<ServiceEndpointCandidate> candidateSet = serviceEndpointCandidatesProvider.getEndpointCandidates();
        for (URI staleCandidateUri : serviceEndpointsProcessor.getKnownUrisNotPresentInFreshCandidateSet(candidateSet)) {
            ServiceEndpoint endpointToCheck = serviceEndpointsProcessor.getKnownServiceByUri(staleCandidateUri);
            if (endpointToCheck != null) {
                removeServiceEndpointIfItBecameUnavailable(endpointToCheck);
            } else {
                logger.t("Service with URI {} might already disappeared during ongoing recheck", staleCandidateUri);
            }
        }
        serviceEndpointsProcessor.removeFailedServicesNotPresentInFreshCandidateSet(candidateSet);
        serviceEndpointsProcessor.removeUpdatedCandidatesFromFailedServices(candidateSet);

        List<ServiceEndpointCandidate> candidatesForPoll = serviceEndpointsProcessor.getFreshCandidates(candidateSet);
        candidatesForPoll.forEach(this::detectServiceUsingServiceEndpointCandidate);
    }

    public void reCheckForFailedUris() {
        serviceEndpointsProcessor.updateServicesListForReCheck();
    }

    public ServiceEndpoint getServiceEndpointForUuid(UUID serviceUuid) {
        return serviceEndpointsProcessor.getServiceEndpointForUuid(serviceUuid);
    }

    public void removeServiceEndpointIfItBecameUnavailable(ServiceEndpoint serviceEndpoint) {
        if (!serviceDetection.isServiceAvailable(serviceEndpoint)) {
            serviceDetectionListener.onServiceRemoved(serviceEndpoint);
            serviceEndpointsProcessor.removeKnownService(serviceEndpoint.getEndpointUri());
        }
    }

    /**
     * Method initiates retry of service detection for previously failed URIs
     */
    public void retryFailedEndpointCandidates() {
        serviceEndpointsProcessor.getCandidatesForRetry().forEach(this::detectServiceUsingServiceEndpointCandidate);
    }

    /**
     * Method tries to detect a single service using service endpoint candidate
     * and notify discovery service upon success.
     *
     * Detected service is added to knownLocations so it won't be unnecessarily polled later.
     *
     * @param candidate
     */
    private void detectServiceUsingServiceEndpointCandidate(ServiceEndpointCandidate candidate) {
        try {
            ServiceEndpoint serviceEndpoint = serviceDetection.getServiceEndpoint(candidate);
            serviceDetectionListener.onServiceDetected(serviceEndpoint);
            serviceEndpointsProcessor.addKnownEndpointService(serviceEndpoint);
            serviceEndpointsProcessor.removeCandidateFromFailedServices(candidate);
        } catch (ServiceDetectionException e) {
            serviceEndpointsProcessor.failServiceEndpointCandidate(candidate);
        }
    }

    public void filterKnownEndpoints(Set<UUID> knownUuidSet) {
        Set<ServiceEndpoint> knownEndpoints = knownUuidSet.stream()
                .map(serviceEndpointsProcessor::getServiceEndpointForUuid)
                .collect(toSet());

        serviceEndpointsProcessor.filterKnownServices(knownEndpoints);
    }
}
