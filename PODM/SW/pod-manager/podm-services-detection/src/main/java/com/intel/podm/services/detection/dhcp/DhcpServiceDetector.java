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

package com.intel.podm.services.detection.dhcp;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.Dhcp;
import com.intel.podm.services.detection.dhcp.tasks.ProvideEndpointsScheduledTask;
import com.intel.podm.services.detection.dhcp.tasks.RecheckFailedUrisScheduledTask;

import javax.annotation.PostConstruct;
import javax.annotation.Resource;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.inject.Inject;
import java.util.concurrent.RejectedExecutionException;

import static com.intel.podm.common.types.discovery.DiscoveryProtocols.DHCP;
import static java.util.concurrent.TimeUnit.SECONDS;

@Startup
@Singleton
@DependsOn({"DiscoveryStartup"})
public class DhcpServiceDetector {

    @Resource
    ManagedScheduledExecutorService managedExecutorService;

    @Inject
    private Logger logger;

    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configuration;

    @Inject
    private ServiceChecker serviceChecker;

    @Inject
    private ProvideEndpointsScheduledTask provideEndpointsScheduledTask;

    @Inject
    private RecheckFailedUrisScheduledTask recheckFailedUrisScheduledTask;

    @PostConstruct
    private void init() {

        if (!configuration.get().isProtocolEnabled(DHCP)) {
            logger.t("DHCP protocol is disabled");
            return;
        }

        logger.i("Initializing DHCP based service detector...");

        Dhcp dhcpConf = configuration.get().getProtocols().getDhcp();

        try {
            managedExecutorService.scheduleWithFixedDelay(provideEndpointsScheduledTask,
                    dhcpConf.getFilesCheckIntervalInSeconds(),
                    dhcpConf.getFilesCheckIntervalInSeconds(), SECONDS);

            managedExecutorService.scheduleWithFixedDelay(recheckFailedUrisScheduledTask,
                    configuration.get().getFailedEndpointRecheckInterval(),
                    configuration.get().getFailedEndpointRecheckInterval(), SECONDS);
        } catch (RejectedExecutionException e) {
            logger.e("Application failed to start properly. Service polling is disabled.", e);
            throw e;
        }
    }

}
