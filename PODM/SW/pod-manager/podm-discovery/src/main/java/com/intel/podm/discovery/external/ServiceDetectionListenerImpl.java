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

package com.intel.podm.discovery.external;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.UUID;

@ApplicationScoped
public class ServiceDetectionListenerImpl implements ServiceDetectionListener { // Do not change form of logs of method onServiceDetected!
    @Inject
    private Logger logger;

    @Inject
    private ServiceExplorer serviceExplorer;

    @Inject
    private ExternalServiceUpdater externalServiceUpdater;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Override
    public void onServiceDetected(ServiceEndpoint serviceEndpoint) {
        UUID serviceUuid = serviceEndpoint.getServiceUuid();
        taskCoordinator.registerAsync(serviceUuid, () -> {
            logger.i("Service {} detected", serviceEndpoint);
            externalServiceUpdater.updateExternalService(serviceEndpoint);
            serviceExplorer.startMonitoringOfService(serviceUuid);
        });
    }

    @Override
    public void onServiceRemoved(UUID serviceUuid) {
        serviceExplorer.enqueueVerification(serviceUuid);
    }
}
