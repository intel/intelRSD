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

package com.intel.rsd.resourcemanager.runner;

import com.intel.rsd.resourcemanager.common.CacheUpdater;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.services.common.hazelcast.ScheduledWithFixedDelayOnLeaderTask;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.time.Duration;

@Component
@Slf4j
public class RefreshCacheOnLeaderTask extends ScheduledWithFixedDelayOnLeaderTask {
    private final long refreshInSeconds;
    private final DetectedManagersCache detectedManagersCache;
    private final CacheUpdater cacheUpdater;

    @Autowired
    public RefreshCacheOnLeaderTask(TaskScheduler taskScheduler,
                                    @Value("${detected.service.provider.cache-refresh-rate-in-second:5}") long seconds,
                                    DetectedManagersCache detectedManagersCache,
                                    CacheUpdater cacheUpdater) {
        super(taskScheduler);
        this.refreshInSeconds = seconds;
        this.detectedManagersCache = detectedManagersCache;
        this.cacheUpdater = cacheUpdater;
    }

    @Override
    protected Duration getFixedDelay() {
        return Duration.ofSeconds(refreshInSeconds);
    }

    @Override
    public String toString() {
        return "Resource Manager Manager's Cache Refresher";
    }

    @Override
    public void run() {
        detectedManagersCache.update(cacheUpdater);
    }
}
