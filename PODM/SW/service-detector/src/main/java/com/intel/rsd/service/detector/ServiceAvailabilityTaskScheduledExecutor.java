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

import com.intel.rsd.service.detector.data.Service;
import com.intel.rsd.service.detector.data.ServiceRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;

import static java.util.concurrent.TimeUnit.SECONDS;

@Component
@Slf4j
public class ServiceAvailabilityTaskScheduledExecutor {
    private final ThreadPoolExecutor threadPoolTaskExecutor;
    private final ServiceRepository serviceRepository;
    private final ServiceAvailabilityCheckerTaskFactory serviceAvailabilityCheckerTaskFactory;
    private final BlockingQueue<Runnable> blockingQueueForExecutor;

    @Autowired
    ServiceAvailabilityTaskScheduledExecutor(ServiceAvailabilityCheckerConfiguration serviceAvailabilityCheckerConfiguration,
                                             ServiceAvailabilityCheckerTaskFactory serviceAvailabilityCheckerTaskFactory,
                                             ServiceRepository serviceRepository) {
        this.serviceAvailabilityCheckerTaskFactory = serviceAvailabilityCheckerTaskFactory;
        this.serviceRepository = serviceRepository;

        ServiceAvailabilityCheckerConfiguration.ThreadPoolExecutorConfig executorConfig = serviceAvailabilityCheckerConfiguration.getThreadPoolExecutor();
        this.blockingQueueForExecutor = new LinkedBlockingQueue<>(executorConfig.getQueueCapacity());
        this.threadPoolTaskExecutor = new ThreadPoolExecutor(
            executorConfig.getMinThreads(),
            executorConfig.getMaxThreads(),
            executorConfig.getKeepAliveTimerSeconds(), SECONDS,
            blockingQueueForExecutor
        );
        log.info("Started service availability check with config: {}", serviceAvailabilityCheckerConfiguration);
    }

    @Scheduled(fixedDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${service-availability.recheck-interval-seconds:30})}")
    public void checkAllServices() {
        log.info("Started availability check");
        for (Service service : serviceRepository.findAll()) {
            ServiceAvailabilityCheckerTask serviceAvailabilityCheckerTask = serviceAvailabilityCheckerTaskFactory.createTask(service.getId());
            if (!blockingQueueForExecutor.contains(serviceAvailabilityCheckerTask)) {
                log.trace("Checking service({})", service.getServiceUri());
                threadPoolTaskExecutor.execute(serviceAvailabilityCheckerTask);
            }
        }
        log.info("Finished availability check");
    }
}
