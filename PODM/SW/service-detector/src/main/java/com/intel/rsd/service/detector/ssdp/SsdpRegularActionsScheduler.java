/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.service.detector.ssdp;

import org.springframework.scheduling.annotation.Scheduled;

public class SsdpRegularActionsScheduler {

    private SearchRedfishServiceTask provideEndpointsScheduledTask;

    public SsdpRegularActionsScheduler(SearchRedfishServiceTask provideEndpointsScheduledTask) {
        this.provideEndpointsScheduledTask = provideEndpointsScheduledTask;
    }

    @Scheduled(fixedDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${protocols.ssdp.AnnouncementFrequencyInSeconds})}")
    private void schedule() {
        provideEndpointsScheduledTask.run();
    }
}
