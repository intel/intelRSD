/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.service.detector.endpoint.verification;

import com.intel.rsd.service.detector.data.Service;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;

import static com.intel.rsd.service.detector.data.Service.State.Enabled;

@Slf4j
@org.springframework.stereotype.Service
public class VerifyServicesTask implements Runnable {
    private final ServiceRepository serviceRepository;
    private final ServiceVerifier serviceVerifier;

    @Autowired
    public VerifyServicesTask(ServiceRepository serviceRepository, ServiceVerifier serviceVerifier) {
        this.serviceRepository = serviceRepository;
        this.serviceVerifier = serviceVerifier;
    }

    @Scheduled(
        initialDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${untrastedservicesverifier.initialDelayInSeconds:5})}",
        fixedRateString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${untrastedservicesverifier.fixedRateInSeconds:5})}"
    )
    public void run() {
        serviceRepository.findAllByStatus(Enabled).forEach(this::verify);
    }

    @SuppressWarnings("checkstyle:IllegalCatch")
    private void verify(Service service) {
        log.debug("Verifying service {}", service);
        try {
            boolean isTrusted = serviceVerifier.verify(service);
            if (!isTrusted) {
                log.warn("Service {} cannot be verified", service.getServiceUri());
            }
            if (!serviceRepository.setTrusted(service.getId(), isTrusted)) {
                log.error("State of service entity {} cannot be updated", service);
            }
        } catch (Exception e) {
            log.error("Unexpected exception during {} verification", service);
        }
    }

}
