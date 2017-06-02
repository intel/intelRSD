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

package com.intel.podm.discovery;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.ExternalServiceSanitizer;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

@Singleton
@Startup
@DependsOn({"ResourceProvider", "PodStartupDiscovery"})
public class DiscoveryStartup {
    @Inject
    private Logger logger;

    @Inject
    private ExternalServiceSanitizer externalServiceSanitizer;

    @PostConstruct
    public void initialize() {
        // TODO: fix me: use JPA Listeners
        logger.d("Sanitizing existing external services");
        externalServiceSanitizer.sanitizeExistingServices();
        externalServiceSanitizer.scheduleServicesDiscovery();
    }
}
