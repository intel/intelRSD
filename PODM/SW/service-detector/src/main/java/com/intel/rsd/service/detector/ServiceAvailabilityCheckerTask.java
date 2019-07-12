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

package com.intel.rsd.service.detector;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestTemplate;

import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.rsd.service.detector.data.Service.State.Enabled;
import static com.intel.rsd.service.detector.data.Service.State.UnavailableOffline;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Slf4j
public class ServiceAvailabilityCheckerTask implements Runnable {
    private final RestTemplate restTemplate;
    private final ServiceRepository serviceRepository;
    private final String serviceODataId;

    ServiceAvailabilityCheckerTask(RestTemplate restTemplate, ServiceRepository serviceRepository, String serviceODataId) {
        this.restTemplate = restTemplate;
        this.serviceRepository = serviceRepository;
        this.serviceODataId = serviceODataId;
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public void run() {
        val optService = serviceRepository.findById(serviceODataId);
        if (optService.isPresent()) {
            val service = optService.get();
            try {
                restTemplate.getForEntity(service.getServiceUri(), JsonNode.class);
                serviceRepository.setServiceState(service.getId(), Enabled);
            } catch (ResourceAccessException exc) {
                serviceRepository.setServiceState(service.getId(), UnavailableOffline);
                return;
            }
        } else {
            log.debug("Failed to find {} representation in database, probably it was already removed", serviceODataId);
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        ServiceAvailabilityCheckerTask that = (ServiceAvailabilityCheckerTask) o;
        return serviceODataId.equals(that.serviceODataId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(serviceODataId);
    }
}
