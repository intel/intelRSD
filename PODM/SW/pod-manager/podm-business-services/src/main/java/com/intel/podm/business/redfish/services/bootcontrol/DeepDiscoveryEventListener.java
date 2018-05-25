/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.ExternalServiceMonitoringEvent;
import com.intel.podm.discovery.external.finalizers.DeepDiscoveryCompletedEvent;

import javax.enterprise.context.Dependent;
import javax.enterprise.event.Observes;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.enterprise.event.TransactionPhase.AFTER_COMPLETION;
import static javax.enterprise.event.TransactionPhase.BEFORE_COMPLETION;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Dependent
public class DeepDiscoveryEventListener {
    @Inject
    private DeepDiscovery deepDiscovery;

    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfigHolder;

    @Transactional(SUPPORTS)
    public void onExternalServiceMonitoringEvent(@Observes(during = AFTER_COMPLETION) ExternalServiceMonitoringEvent event) {
        switch (event.getMonitoringState()) {
            case STARTED:
                DiscoveryConfig discoveryConfig = discoveryConfigHolder.get();
                if (discoveryConfig.isDeepDiscoveryEnabled()) {
                    deepDiscovery.triggerForExternalService(event.getExternalServiceUuid());
                }
                break;
            case STOPPED:
                deepDiscovery.cancelForExternalService(event.getExternalServiceUuid());
                break;
            default:
                break;
        }
    }

    @Transactional(MANDATORY)
    public void onBeforeDeepDiscoveryCompletion(@Observes(during = BEFORE_COMPLETION) DeepDiscoveryCompletedEvent event) {
        deepDiscovery.finalizeForComputerSystem(event.getComputerSystemId());
    }
}
