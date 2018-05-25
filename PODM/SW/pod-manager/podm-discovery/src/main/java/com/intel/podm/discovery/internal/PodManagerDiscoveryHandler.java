/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.embeddables.CommandShell;
import com.intel.podm.business.entities.redfish.embeddables.GraphicalConsole;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.embeddables.SerialConsole;
import com.intel.podm.common.types.Status;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.version.VersionLoader;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.Collection;

import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.ManagerType.MANAGEMENT_CONTROLLER;
import static com.intel.podm.common.types.PowerState.ON;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.common.utils.IterableHelper.single;

@ApplicationScoped
class PodManagerDiscoveryHandler {
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
        requiresNonNull(podChassis, "podChassis");

        Collection<Manager> podManagers = podChassis.getManagers();
        if (!podManagers.isEmpty()) {
            Manager manager = single(podManagers);
            updateManagerForPod(manager);
            return manager;
        } else {
            Manager manager = createManagerForPod(podChassis);
            podChassis.addManager(manager);
            return manager;
        }
    }

    private Manager createManagerForPod(Chassis podChassis) {
        Manager manager = genericDao.create(Manager.class);

        manager.setFirmwareVersion(versionLoader.loadAppVersion());

        manager.setId(id("pod-manager"));
        manager.setStatus(new Status(ENABLED, OK, OK));
        manager.setName("Pod Manager");
        manager.setGraphicalConsole(createDisabledGraphicalConsole());
        manager.setSerialConsole(createDisabledSerialConsole());
        manager.setCommandShell(createDisabledCommandShell());
        manager.setManagerType(MANAGEMENT_CONTROLLER);
        manager.setInChassisManager(podChassis);
        manager.setServiceEntryPointUuid(config.get().getUuid());
        manager.setPowerState(ON);
        podManagerNetworkDiscoveryHandler.initializeNetworking(manager);

        return manager;
    }

    private void updateManagerForPod(Manager manager) {
        manager.setFirmwareVersion(versionLoader.loadAppVersion());
    }

    private GraphicalConsole createDisabledGraphicalConsole() {
        GraphicalConsole graphicalConsole = new GraphicalConsole();
        graphicalConsole.setServiceEnabled(false);
        return graphicalConsole;
    }

    private SerialConsole createDisabledSerialConsole() {
        SerialConsole serialConsole = new SerialConsole();
        serialConsole.setServiceEnabled(false);
        return serialConsole;
    }

    private CommandShell createDisabledCommandShell() {
        CommandShell commandShell = new CommandShell();
        commandShell.setServiceEnabled(false);
        return commandShell;
    }
}
