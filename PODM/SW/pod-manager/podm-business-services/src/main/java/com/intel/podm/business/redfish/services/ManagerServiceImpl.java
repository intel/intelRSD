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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ManagerDto;
import com.intel.podm.business.dto.redfish.attributes.ConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.GraphicalConsoleDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.Console;
import com.intel.podm.business.entities.redfish.properties.GraphicalConsole;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ManagerService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGERS;
import static java.util.Objects.nonNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ManagerServiceImpl implements ManagerService {
    @Inject
    DomainObjectTreeTraverser traverser;

    @Inject
    GenericDao genericDao;

    @Override
    public CollectionDto getManagerCollection() {
        List<Manager> managerCollection = genericDao.findAll(Manager.class);
        return new CollectionDto(MANAGERS, Contexts.getAsIdList(managerCollection));
    }

    @Override
    public ManagerDto getManager(Context context) throws EntityNotFoundException {
        Manager manager = (Manager) traverser.traverse(context);
        return map(manager);
    }

    private ManagerDto map(Manager manager) {
        return ManagerDto.newBuilder()
                .id(manager.getId())
                .name(manager.getName())
                .managerType(manager.getManagerType())
                .description(manager.getDescription())
                .serviceEntryPointUuid(manager.getServiceEntryPointUuid())
                .uuid(manager.getUuid())
                .model(manager.getModel())
                .status(manager.getStatus())
                .graphicalConsole(toConsoleDto(manager.getGraphicalConsole()))
                .serialConsole(toConsoleDto(manager.getSerialConsole()))
                .commandShell(toConsoleDto(manager.getCommandShell()))
                .firmwareVersion(manager.getFirmwareVersion())
                .hasNetworkProtocol(manager.getNetworkProtocol() != null)
                .context(Contexts.toContext(manager))
                .managedComputerSystems(Contexts.asComputerSystemContexts(manager.getManagedComputerSystems()))
                .managedChassisCollection(Contexts.asChassisContexts(manager.getManagedChassisCollection()))
                .managerInChassis(nonNull(manager.getManagerInChassis()) ? Contexts.toContext(manager.getManagerInChassis()) : null)
                .managedEthernetSwitches(Contexts.asEthernetSwitchContexts(manager.getManagedEthernetSwitches()))
                .managedServices(Contexts.asStorageServicesContexts(manager.getManagedServices()))
                .build();
    }

    public ConsoleDto toConsoleDto(Console console) {
        if (console == null) {
            return null;
        }

        return ConsoleDto
                .newBuilder()
                .serviceEnabled(console.isServiceEnabled())
                .maxConcurrentSessions(console.getMaxConcurrentSessions())
                .connectTypesSupported(console.getConnectTypesSupported())
                .build();
    }

    public GraphicalConsoleDto toConsoleDto(GraphicalConsole console) {
        if (console == null) {
            return null;
        }

        return GraphicalConsoleDto
                .newBuilder()
                .serviceEnabled(console.isServiceEnabled())
                .maxConcurrentSessions(console.getMaxConcurrentSessions())
                .connectTypesSupported(console.getConnectTypesSupported())
                .build();
    }
}
