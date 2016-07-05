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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Status;

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
@DependsOn("DomainObjectModelStartup")
@TransactionAttribute(TransactionAttributeType.REQUIRED)
public class PodStartupDiscovery {
    public static final String DEFAULT_POD_LOCATION_COORDINATE = "Pod1";
    private static final String POD_DEFAULT_NAME = "POD";

    @Inject
    private PodManagerDiscoveryHandler podManagerDiscoveryHandler;

    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ChassisDao chassisDao;

    @PostConstruct
    public void initInitialPod() {
        Chassis podChassis = singleOrNull(chassisDao.getAllByChassisType(POD));
        if (podChassis == null) {
            logger.d("Creating POD at location {}", DEFAULT_POD_LOCATION_COORDINATE);
            podChassis = createPod(DEFAULT_POD_LOCATION_COORDINATE);
        }
        podManagerDiscoveryHandler.getManagerForPod(podChassis);
    }

    private Chassis createPod(String podLocation) {
        Chassis pod = chassisDao.create();

        pod.setName(POD_DEFAULT_NAME);
        pod.setChassisType(POD);
        pod.setLocationId(podLocation);
        pod.setStatus(new Status(ENABLED, OK, null));

        return pod;
    }
}
