/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.version.VersionLoader;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

@Singleton
@Startup
@DependsOn({"DiscoveryStartup", "ServiceDetectionStartup", "MetricRegistryFactory"})
public class ApplicationStartup {
    @Inject
    private Logger logger;

    @Inject
    private VersionLoader versionLoader;

    @PostConstruct
    private void initialize() {
        logger.i("PodM started (version: {})", versionLoader.loadAppVersion());
    }

    @PreDestroy
    private void shutdown() {
        logger.i("PodM is shutting down (version: {})", versionLoader.loadAppVersion());
    }
}
