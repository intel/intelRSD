/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.detection.ServiceChecker;
import com.intel.podm.services.detection.ServiceEndpoint;
import com.intel.podm.services.detection.ServiceEndpointChecker;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRED;

@ApplicationScoped
@Transactional(REQUIRED)
public class ExternalServiceAvailabilityChecker {

    @Inject
    private Logger logger;

    @Inject
    private ExternalServiceRepository serviceRepository;

    @Inject
    private ServiceEndpointChecker serviceEndpointChecker;

    @Inject
    private ServiceChecker serviceChecker;

    public void verifyServiceAvailabilityByUuid(UUID serviceUuid) {
        serviceChecker.filterKnownEndpoints(serviceRepository.getAllServiceUuids());
        Set<ServiceEndpoint> endpointsToCheck = new HashSet<>();
        ServiceEndpoint endpoint = serviceChecker.getServiceEndpointForUuid(serviceUuid);
        if (endpoint != null) {
            endpointsToCheck.add(endpoint);
        } else {
            logger.w("UUID {} is not a known endpoint in service detection module", serviceUuid);
        }

        ServiceEndpoint storedEndpoint = getFromExternalServiceRepository(serviceUuid);
        if (storedEndpoint != null) {
            endpointsToCheck.add(storedEndpoint);
        } else {
            logger.w("UUID {} is not present in external service repository", serviceUuid);
        }

        triggerCheckOnServices(endpointsToCheck);
   }

    private void triggerCheckOnServices(Set<ServiceEndpoint> endpointsToCheck) {
        endpointsToCheck.forEach(serviceEndpointChecker::triggerCheckIfServiceStillExist);
    }

    private ServiceEndpoint getFromExternalServiceRepository(UUID externalServiceUuid) {
        ExternalService service = serviceRepository.findOrNull(externalServiceUuid);
        return (service != null)
            ? new ServiceEndpoint(service.getServiceType(), service.getUuid(), service.getBaseUri(), service.getMacAddress())
            : null;
    }
}
