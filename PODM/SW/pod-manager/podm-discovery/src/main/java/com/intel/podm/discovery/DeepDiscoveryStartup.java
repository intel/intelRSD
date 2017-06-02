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

package com.intel.podm.discovery;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.deep.ComputerSystemSanitizer;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

@Singleton
@Startup
@DependsOn({"DiscoveryStartup"})
public class DeepDiscoveryStartup {
    @Inject
    private Logger logger;

    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    @Inject
    private ComputerSystemSanitizer computerSystemSanitizer;

    @PostConstruct
    public void initialize() {
        if (discoveryConfig.get().isDeepDiscoveryEnabled()) {
            logger.d("Reinitializing deep discovery process on computer systems that were left in DeepInProgress state");
            computerSystemSanitizer.sanitizeComputerSystemsUsedForDeepDiscovery();
        }
    }
}
