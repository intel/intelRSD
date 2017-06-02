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

package com.intel.podm.discovery.external.event;

import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.discovery.external.DiscoveryScheduler;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import java.util.UUID;

@RequestScoped
public class EventSubscriptionTask extends DefaultManagedTask implements Runnable {

    @Inject
    private EventSubscribeRunner eventSubscribeRunner;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    private UUID serviceUuid;

    public void setServiceUuid(UUID serviceUuid) {
        this.serviceUuid = serviceUuid;
    }

    @Override
    public void run() {
        boolean subscribed = eventSubscribeRunner.subscribeIfNotAlreadySubscribed(serviceUuid);
        if (subscribed) {
            discoveryScheduler.enqueueDiscovery(serviceUuid);
        }
    }
}
