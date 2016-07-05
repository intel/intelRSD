/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.properties.Boot;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.DONE;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.INITIAL;
import static com.intel.podm.common.types.DiscoveryState.BASIC;
import static com.intel.podm.common.types.DiscoveryState.DEEP;

@Dependent
public class ComputerSystemMapper extends DomainObjectMapper<ComputerSystemResource, ComputerSystem> {
    @Inject
    private GenericDao genericDao;

    public ComputerSystemMapper() {
        super(ComputerSystemResource.class, ComputerSystem.class);
        registerProvider(Boot.class, source -> provideBoot());
    }

    @Override
    protected void performNotAutomatedMapping(ComputerSystemResource source, ComputerSystem target) {
        DiscoveryState state = target.getDiscoveryState();

        if (!source.isBasic()) {
            target.setDiscoveryState(DEEP);
            target.setDeepDiscoveryState(DONE);
        } else if (state == null) {
            target.setDiscoveryState(BASIC);
            target.setDeepDiscoveryState(INITIAL);
        }
    }

    private Boot provideBoot() {
        Boot boot = target.getBoot();

        if (boot == null) {
            boot = genericDao.create(Boot.class);
        }

        return boot;
    }
}
