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

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import org.fourthline.cling.UpnpService.Start;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.enterprise.event.Event;
import javax.inject.Inject;

import static com.intel.podm.common.types.discovery.DiscoveryProtocols.SSDP;

@Startup
@Singleton
@DependsOn({"DiscoveryStartup"})
public class SsdpServiceDetector {
    public static final String AL_HEADER = "AL";

    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configuration;

    @Inject
    private Event<Start> upnpServiceStartEvent;

    @Inject
    private Logger logger;

    @Inject
    private SsdpRegularActionsScheduler regularActionsScheduler;

    @PostConstruct
    private void init() {

        if (!configuration.get().isProtocolEnabled(SSDP)) {
            logger.t("SSDP protocol is disabled.");
            return;
        }

        logger.i("Initializing SSDP based service detector...");
        upnpServiceStartEvent.fire(new Start());
        regularActionsScheduler.schedule();
    }
}
