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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.Console;
import com.intel.podm.business.entities.redfish.properties.GraphicalConsole;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.version.VersionLoader;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.Collection;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.ManagerType.MANAGEMENT_CONTROLLER;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.utils.IterableHelper.single;

@ApplicationScoped
public class PodManagerDiscoveryHandler {

    @Inject
    GenericDao genericDao;

    @Inject
    PodManagerNetworkDiscoveryHandler podManagerNetworkDiscoveryHandler;

    @Inject
    VersionLoader versionLoader;

    @Inject
    @Config
    Holder<ServiceConfig> config;

    public Manager getManagerForPod(Chassis podChassis) {
        if (podChassis == null) {
            throw new IllegalArgumentException("Cannot create Manager for a null Pod");
        }

        Collection<Manager> podManagers = podChassis.getManagers();
        if (!podManagers.isEmpty()) {
            return single(podManagers);
        } else {
            Manager manager = createManagerForPod(podChassis);
            podChassis.addManager(manager);
            return manager;
        }
    }

    private Manager createManagerForPod(Chassis podChassis) {
        Manager manager = genericDao.create(Manager.class);

        manager.setFirmwareVersion(versionLoader.loadAppVersion());

        manager.setStatus(new Status(ENABLED, OK, OK));
        manager.setName("POD Manager");

        manager.setGraphicalConsole(createDisabledGraphicalConsole());
        manager.setSerialConsole(createDisabledConsole());
        manager.setCommandShell(createDisabledConsole());
        manager.setManagerType(MANAGEMENT_CONTROLLER);
        manager.setManagerInChassis(podChassis);
        manager.setServiceEntryPointUuid(config.get().getUuid());
        podManagerNetworkDiscoveryHandler.initializeNetworking(manager);

        return manager;
    }

    private Console createDisabledConsole() {
        Console console = genericDao.create(Console.class);
        console.setServiceEnabled(false);
        return console;
    }

    private GraphicalConsole createDisabledGraphicalConsole() {
        GraphicalConsole console = genericDao.create(GraphicalConsole.class);
        console.setServiceEnabled(false);
        return console;
    }
}
