/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.services.detection.ssdp;

import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.Ssdp;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.concurrent.TimeUnit.SECONDS;

@Dependent
public class SsdpRegularActionsScheduler {
    private static final long NO_DELAY = 0;

    @Resource
    private ManagedScheduledExecutorService managedExecutorService;

    @Inject
    private SearchRedfishServiceTask provideEndpointsScheduledTask;

    void schedule(Ssdp ssdp) {
        managedExecutorService.scheduleWithFixedDelay(
            provideEndpointsScheduledTask,
            NO_DELAY,
            ssdp.getAnnouncementFrequencyInSeconds(),
            SECONDS
        );
    }
}
