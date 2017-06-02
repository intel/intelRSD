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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.inject.Inject;

import static com.intel.podm.common.types.ChassisType.POD;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;

@Singleton
@Startup
@DependsOn("DatabaseSchemaUpdateFinalizer")
@TransactionAttribute(TransactionAttributeType.REQUIRED)
public class PodStartupDiscovery {
    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    @Inject
    private PodManagerDiscoveryHandler podManagerDiscoveryHandler;

    @Inject
    private Logger logger;

    @Inject
    private ChassisDao chassisDao;

    @PostConstruct
    private void initInitialPod() {
        logger.i("Initializing {}", getClass().getSimpleName());
        Chassis podChassis = singleOrNull(chassisDao.getAllByChassisType(POD));
        if (podChassis == null) {
            String podLocation = discoveryConfig.get().getPodLocationId();
            logger.d("Creating POD at location {}", podLocation);
            podChassis = createPod(podLocation);
        }
        podManagerDiscoveryHandler.getManagerForPod(podChassis);
    }

    private Chassis createPod(String podLocation) {
        Chassis pod = chassisDao.create();

        pod.setName(discoveryConfig.get().getPodName());
        pod.setChassisType(POD);
        pod.setLocationId(podLocation);
        pod.setId(Id.id("pod"));
        pod.setStatus(new Status(ENABLED, OK, null));

        return pod;
    }
}
