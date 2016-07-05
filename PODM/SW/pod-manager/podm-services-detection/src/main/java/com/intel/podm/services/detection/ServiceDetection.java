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

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import java.util.UUID;

/**
 * Service Detection implementation based on obtaining UUID of service
 * being detected at given URI using REST client.
 */
@RequestScoped
class ServiceDetection {

    @Inject
    private ServiceRestClient serviceRestClient;

    @Inject
    private Logger logger;

    ServiceEndpoint getServiceEndpoint(ServiceEndpointCandidate candidate) throws ServiceDetectionException {
        UUID serviceUuid = serviceRestClient.obtainServiceUuidFromCandidate(candidate);
        return new ServiceEndpoint(candidate.getServiceType(), serviceUuid, candidate.getEndpointUri(), candidate.getMacAddress());
    }

    boolean isServiceAvailable(ServiceEndpoint serviceEndpoint) {
        try {
            serviceRestClient.obtainServiceUuidFromService(serviceEndpoint);
        } catch (ServiceDetectionException e) {
            logger.i("Service {} is not available: {}", serviceEndpoint, e.getMessage());
            return false;
        }
        return true;
    }
}
