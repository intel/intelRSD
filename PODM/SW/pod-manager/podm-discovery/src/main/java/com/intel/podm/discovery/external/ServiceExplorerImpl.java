/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.discovery.ServiceExplorer;

import javax.ejb.Stateless;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.SUPPORTS;

@Stateless
public class ServiceExplorerImpl implements ServiceExplorer {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private ExternalServiceMonitor externalServiceMonitor;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Override
    @Transactional(SUPPORTS)
    public void discover(UUID serviceUuid) {
        DiscoveryRunner discoveryRunner = beanFactory.create(DiscoveryRunner.class);
        discoveryRunner.setServiceUuid(serviceUuid);
        discoveryRunner.run();
    }

    @Override
    @Transactional(SUPPORTS)
    public void startMonitoringOfService(UUID serviceUuid) {
        externalServiceMonitor.monitorService(serviceUuid);
    }

    @Override
    @Transactional(SUPPORTS)
    public void enqueueVerification(UUID serviceUuid) {
        availabilityChecker.verifyServiceAvailabilityByUuid(serviceUuid);
    }

    @Override
    @Transactional(SUPPORTS)
    public void forgetService(UUID serviceUuid) {
        externalServiceMonitor.stopMonitoringOfService(serviceUuid);
    }
}
