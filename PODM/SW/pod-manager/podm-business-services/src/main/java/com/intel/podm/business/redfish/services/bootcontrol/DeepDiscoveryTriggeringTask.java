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

package com.intel.podm.business.redfish.services.bootcontrol;

import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.NOT_SUPPORTED;

/**
 * Class used to control DeepDiscovery execution
 */
@Dependent
@Transactional(NOT_SUPPORTED)
@SuppressWarnings({"checkstyle:IllegalCatch"})
class DeepDiscoveryTriggeringTask extends DefaultManagedTask implements Runnable {

    @Inject
    private Logger logger;

    @Inject
    private DeepDiscoveryComputerSystemSelector selector;

    @Inject
    private DeepDiscoveryRunner runner;

    private UUID serviceUuid;

    @Override
    public void run() {
        try {
            requiresNonNull(serviceUuid, "serviceUuid");
            selector.markComputerSystemsForDeepDiscovery(serviceUuid);
            runner.deepDiscoverComputerSystems(serviceUuid);
        } catch (RuntimeException e) {
            logger.e("DeepDiscoveryTriggeringTask error: " + e.getMessage(), e);
        }
    }

    public void setServiceUuid(UUID serviceUuid) {
        this.serviceUuid = serviceUuid;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        DeepDiscoveryTriggeringTask that = (DeepDiscoveryTriggeringTask) o;
        return Objects.equals(serviceUuid, that.serviceUuid);
    }

    @Override
    public int hashCode() {
        return Objects.hash(serviceUuid);
    }
}
