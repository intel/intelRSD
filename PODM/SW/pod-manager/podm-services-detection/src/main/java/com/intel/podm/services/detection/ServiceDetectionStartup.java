/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.services.detection;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.services.detection.dhcp.DhcpServiceDetector;
import com.intel.podm.services.detection.ssdp.SsdpServiceDetector;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

import static com.intel.podm.common.types.discovery.DiscoveryProtocols.DHCP;
import static com.intel.podm.common.types.discovery.DiscoveryProtocols.SSDP;

@Startup
@Singleton
@DependsOn({"DiscoveryStartup"})
public class ServiceDetectionStartup {
    @Inject
    private SsdpServiceDetector ssdpServiceDetector;

    @Inject
    private DhcpServiceDetector dhcpServiceDetector;

    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configuration;

    @Inject
    private Logger logger;

    @PostConstruct
    private void init() {
        ServiceDetectionConfig serviceDetectionConfig = configuration.get();
        if (serviceDetectionConfig.isProtocolEnabled(SSDP)) {
            ssdpServiceDetector.init(serviceDetectionConfig.getProtocols().getSsdp());
        } else {
            logger.t("SSDP protocol is disabled.");
        }

        if (serviceDetectionConfig.isProtocolEnabled(DHCP)) {
            dhcpServiceDetector.init(serviceDetectionConfig.getProtocols().getDhcp());
        } else {
            logger.t("DHCP protocol is disabled");
        }
    }
}
