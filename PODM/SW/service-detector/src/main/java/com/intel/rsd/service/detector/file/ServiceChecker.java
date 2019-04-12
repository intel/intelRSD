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

import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.ServiceDetectionListener;
import com.intel.rsd.service.detector.ServiceEndpoint;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.List;
import java.util.Set;

import static java.util.stream.Collectors.toSet;


/**
 * Class responsible for detecting service presence and notifying
 * discovery service (providing it with service endpoints) if a valid
 * service was detected at given URI. Notifications about known,
 * discovered services are not repeated.
 */
@Slf4j
@Component
@Profile({"dhcp", "serviceslist"})
public class ServiceChecker {
    @Autowired
    private ServiceDetectionListener serviceDetectionListener;

    @Autowired
    private ServiceDescriptor serviceDescriptor;

    @Autowired
    private ServiceEndpointsProcessor serviceEndpointsProcessor;

    @Autowired
    private Collection<ServiceCandidatesProvider> serviceCandidatesProviders;

    /**
     * Method initiates service detection for all provided Service Endpoint Candidates.
     */
    public void triggerEndpointCandidatesCheck() {
        Set<ServiceCandidate> candidateSet = serviceCandidatesProviders.stream()
            .flatMap(ServiceCandidatesProvider::getServiceCandidates).collect(toSet());

        for (URI staleCandidateUri : serviceEndpointsProcessor.getKnownUrisNotPresentInFreshCandidateSet(candidateSet)) {
            ServiceEndpoint endpointToCheck = serviceEndpointsProcessor.getKnownServiceByUri(staleCandidateUri);
            if (endpointToCheck != null) {
                removeServiceEndpointIfItBecameUnavailable(endpointToCheck);
            } else {
                log.trace("Service with URI {} might already disappeared during ongoing recheck", staleCandidateUri);
            }
        }
        serviceEndpointsProcessor.removeFailedServicesNotPresentInFreshCandidateSet(candidateSet);
        serviceEndpointsProcessor.removeUpdatedCandidatesFromFailedServices(candidateSet);

        List<ServiceCandidate> candidatesForPoll = serviceEndpointsProcessor.getFreshCandidates(candidateSet);
        candidatesForPoll.forEach(this::detectServiceUsingServiceEndpointCandidate);
    }

    public void reCheckForFailedUris() {
        serviceEndpointsProcessor.updateServicesListForReCheck();
    }

    public void removeServiceEndpointIfItBecameUnavailable(ServiceEndpoint serviceEndpoint) {
        if (!isServiceAvailable(serviceEndpoint.getEndpointUri())) {
            serviceDetectionListener.onServiceRemoved(serviceEndpoint.getEndpointUri());
            serviceEndpointsProcessor.removeKnownService(serviceEndpoint.getEndpointUri());
        }
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private boolean isServiceAvailable(URI serviceUri) {
        try {
            serviceDescriptor.describe(serviceUri);
            return true;
        } catch (Exception e) {
            log.info("Service {} is not available: {}", serviceUri, e.getMessage());
            return false;
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
     * <p>
     * Detected service is added to knownLocations so it won't be unnecessarily polled later.
     *
     * @param candidate
     */
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private void detectServiceUsingServiceEndpointCandidate(ServiceCandidate candidate) {
        try {
            val se = serviceDescriptor.describe(candidate.getEndpointUri());
            serviceDetectionListener.onServiceDetected(se.getServiceUuid(), se.getEndpointUri(), se.getRemoteServiceType());
            serviceEndpointsProcessor.addKnownEndpointService(se);
            serviceEndpointsProcessor.removeCandidateFromFailedServices(candidate);
        } catch (Exception e) {
            log.error(e.getMessage(), e);
            serviceEndpointsProcessor.failServiceEndpointCandidate(candidate);
        }
    }
}
