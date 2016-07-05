/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.config.version.VersionLoader;
import com.intel.podm.discovery.external.ComposedNodeSanitizer;
import com.intel.podm.discovery.external.ComputerSystemSanitizer;
import com.intel.podm.discovery.external.ExternalServiceSanitizer;
import com.intel.podm.discovery.external.deep.DeepDiscoveryTaskScheduler;
import com.intel.podm.services.detection.ServiceDetector;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

@Singleton
@Startup
@DependsOn({"DomainObjectModelStartup", "PodStartupDiscovery"})
public class ApplicationStartup {

    @Inject
    Logger logger;

    @Inject
    ServiceDetector serviceDetector;

    @Inject
    DeepDiscoveryTaskScheduler deepDiscoveryTaskScheduler;

    @Inject
    VersionLoader versionLoader;

    @Inject @Config
    Holder<DiscoveryConfig> discoveryConfig;

    @Inject
    ExternalServiceSanitizer externalServiceSanitizer;

    @Inject
    ComputerSystemSanitizer computerSystemSanitizer;

    @Inject
    ComposedNodeSanitizer composedNodeSanitizer;

    @PostConstruct
    public void applicationStartup() {
        logger.d("Sanitizing existing external services");
        externalServiceSanitizer.sanitizeExistingServices();

        if (discoveryConfig.get().isDeepDiscoveryEnabled()) {
            logger.d("Reinitializing deep discovery process on computer systems that were left in DeepInProgress state");
            computerSystemSanitizer.sanitizeComputerSystemsUsedForDeepDiscovery();
        }

        logger.d("Failing all composed nodes that were not assembled before restart");
        composedNodeSanitizer.sanitizeComputerSystemsUsedInComposedNodes();

        logger.d("Starting uri polling");
        serviceDetector.pollForServices();

        if (discoveryConfig.get().isDeepDiscoveryEnabled()) {
            logger.d("Starting deep discovery scheduling task");
            deepDiscoveryTaskScheduler.start();
        }

        logger.i("PodM started (version: {})", versionLoader.loadAppVersion());
    }

}
