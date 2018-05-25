/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.services.configuration;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.discovery.ServiceEndpoint;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.common.types.ServiceType.DISCOVERY_SERVICE;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.lang.String.format;

@Dependent
public class DiscoveryServiceDetectionHandler {
    @Inject
    private Logger logger;

    @Inject
    private ConfigurationTasksManager configurationManager;

    public void onServiceDetected(ServiceEndpoint serviceEndpoint) {
        requires(DISCOVERY_SERVICE.equals(serviceEndpoint.getServiceType()), format("Only %s ServiceType is supported.", DISCOVERY_SERVICE));

        logger.i("Detected {} service {}", serviceEndpoint.getServiceType(), serviceEndpoint);
        configurationManager.scheduleConfigurer(serviceEndpoint);
    }

    public void onServiceRemoved(ServiceEndpoint serviceEndpoint) {
        requires(DISCOVERY_SERVICE.equals(serviceEndpoint.getServiceType()), format("Only %s ServiceType is supported.", DISCOVERY_SERVICE));

        logger.i("Removed {} service {}", serviceEndpoint.getServiceType(), serviceEndpoint.getServiceUuid());
        configurationManager.cancelConfigurer(serviceEndpoint.getServiceUuid());
    }
}
