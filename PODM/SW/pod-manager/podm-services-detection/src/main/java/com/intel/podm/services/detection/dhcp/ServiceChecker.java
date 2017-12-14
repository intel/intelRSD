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
import com.intel.podm.discovery.external.ServiceDescriptor;
import com.intel.podm.discovery.external.ServiceDetectionListener;
import com.intel.podm.discovery.external.ServiceEndpoint;
import com.intel.podm.discovery.external.UnrecognizedServiceTypeException;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Set;

import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.SUPPORTS;

/**
 * Class responsible for detecting service presence and notifying
 * discovery service (providing it with service endpoints) if a valid
 * service was detected at given URI. Notifications about known,
 * discovered services are not repeated.
 */
@Singleton
public class ServiceChecker {

    @Inject
    private Logger logger;

    @Inject
    private ServiceDetectionListener serviceDetectionListener;

    @Inject
    private ServiceDescriptor serviceDescriptor;

    @Inject
    private ServiceEndpointsProcessor serviceEndpointsProcessor;

    @Inject
    private CandidatesProvider candidatesProvider;

    /**
     * Method initiates service detection for all provided Service Endpoint Candidates.
     *
     * LockType.WRITE used due to concurrent access to service endpoints processor.
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void triggerEndpointCandidatesCheck() {
        Set<DhcpServiceCandidate> candidateSet = candidatesProvider.getEndpointCandidates();
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

        List<DhcpServiceCandidate> candidatesForPoll = serviceEndpointsProcessor.getFreshCandidates(candidateSet);
        candidatesForPoll.forEach(this::detectServiceUsingServiceEndpointCandidate);
    }

    /**
     * LockType.WRITE used due to concurrent access to service endpoints processor.
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void reCheckForFailedUris() {
        serviceEndpointsProcessor.updateServicesListForReCheck();
    }

    /**
     * Method initiates retry of service detection for previously failed URIs
     *
     * LockType.WRITE used due to concurrent access to service endpoints processor.
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void retryFailedEndpointCandidates() {
        serviceEndpointsProcessor.getCandidatesForRetry().forEach(this::detectServiceUsingServiceEndpointCandidate);
    }

    private void removeServiceEndpointIfItBecameUnavailable(ServiceEndpoint serviceEndpoint) {
        if (!isServiceAvailable(serviceEndpoint.getEndpointUri())) {
            serviceDetectionListener.onServiceRemoved(serviceEndpoint.getServiceUuid());
            serviceEndpointsProcessor.removeKnownService(serviceEndpoint.getEndpointUri());
        }
    }

    private boolean isServiceAvailable(URI serviceUri) {
        try {
            serviceDescriptor.describe(serviceUri);
            return true;
        } catch (UnrecognizedServiceTypeException e) {
            logger.i("Service {} is not available: {}", serviceUri, e.getMessage());
            return false;
        }
    }

    /**
     * Method tries to detect a single service using service endpoint candidate
     * and notify discovery service upon success.
     * <p>
     * Detected service is added to knownLocations so it won't be unnecessarily polled later.
     *
     * @param candidate
     */
    private void detectServiceUsingServiceEndpointCandidate(DhcpServiceCandidate candidate) {
        try {
            ServiceEndpoint serviceEndpoint = serviceDescriptor.describe(candidate.getEndpointUri(), candidate.getServiceType());
            serviceDetectionListener.onServiceDetected(serviceEndpoint);
            serviceEndpointsProcessor.addKnownEndpointService(serviceEndpoint);
            serviceEndpointsProcessor.removeCandidateFromFailedServices(candidate);
        } catch (UnrecognizedServiceTypeException e) {
            serviceEndpointsProcessor.failServiceEndpointCandidate(candidate);
        }
    }
}
