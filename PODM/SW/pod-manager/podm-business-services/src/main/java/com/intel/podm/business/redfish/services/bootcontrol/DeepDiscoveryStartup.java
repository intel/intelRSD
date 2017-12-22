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

package com.intel.podm.business.redfish.services.bootcontrol;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.annotation.PostConstruct;
import javax.ejb.AccessTimeout;
import javax.ejb.DependsOn;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.DeepDiscoveryState.INITIAL;
import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DeepDiscoveryState.WAITING_TO_START;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

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
    private GenericDao genericDao;

    /**
     * This method needs to be executed within a transaction context thus it must be public and by default LockType.WRITE is used implicitly
     * for public methods in singleton bean.
     * In real-life scenario it is called only once and does not need any locking mechanism.
     */
    @PostConstruct
    @Lock(WRITE)
    @Transactional(REQUIRES_NEW)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void initialize() {
        if (discoveryConfig.get().isDeepDiscoveryEnabled()) {
            logger.d("Reinitializing deep discovery process on computer systems that were left in DeepInProgress state");
            genericDao.findAll(ComputerSystem.class).stream()
                .filter(computerSystem -> computerSystem.getMetadata().isInAnyOfStates(WAITING_TO_START, RUNNING))
                .forEach(computerSystem -> computerSystem.getMetadata().setDeepDiscoveryState(INITIAL));
        }
    }
}
