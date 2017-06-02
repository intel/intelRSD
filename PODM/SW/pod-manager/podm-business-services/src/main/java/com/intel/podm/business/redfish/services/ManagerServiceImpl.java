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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ManagerDto;
import com.intel.podm.business.dto.redfish.NetworkProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.ConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.GraphicalConsoleDto;
import com.intel.podm.business.dto.redfish.attributes.ProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.SimpleServiceDiscoveryProtocolDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.embeddables.CommandShell;
import com.intel.podm.business.entities.redfish.embeddables.GraphicalConsole;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.entities.redfish.embeddables.SerialConsole;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGERS;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asComputerSystemContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchContexts;
import static com.intel.podm.business.redfish.ContextCollections.asStorageServicesContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength"})
public class ManagerServiceImpl implements ReaderService<ManagerDto> {
    @Inject
    EntityTreeTraverser traverser;

    @Inject
    UnknownOemTranslator unknownOemTranslator;

    @Inject
    private GenericDao genericDao;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        List<Manager> managerCollection = genericDao.findAll(Manager.class);
        return new CollectionDto(MANAGERS, getAsIdSet(managerCollection));
    }

    @Override
    public ManagerDto getResource(Context context) throws ContextResolvingException {
        Manager manager = (Manager) traverser.traverse(context);
        return mapManager(manager);
    }

    private ManagerDto mapManager(Manager manager) {
        return ManagerDto.newBuilder()
            .id(manager.getId().toString())
            .name(manager.getName())
            .description(manager.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(manager.getService(), manager.getUnknownOems()))
            .managerType(manager.getManagerType())
            .serviceEntryPointUuid(manager.getServiceEntryPointUuid())
            .uuid(manager.getUuid())
            .model(manager.getModel())
            .status(manager.getStatus())
            .powerState(manager.getPowerState())
            .graphicalConsole(toConsoleDto(manager.getGraphicalConsole()))
            .serialConsole(toConsoleDto(manager.getSerialConsole()))
            .commandShell(toConsoleDto(manager.getCommandShell()))
            .firmwareVersion(manager.getFirmwareVersion())
            .networkProtocol(mapNetworkProtocol(manager.getNetworkProtocol()))
            .context(toContext(manager))
            .managedComputerSystems(asComputerSystemContexts(manager.getComputerSystems()))
            .managedChassisCollection(asChassisContexts(manager.getManagedChassis()))
            .managerInChassis(toContext(manager.getInChassisManager()))
            .managedEthernetSwitches(asEthernetSwitchContexts(manager.getEthernetSwitches()))
            .managedServices(asStorageServicesContexts(manager.getStorageServices()))
            .build();
    }

    public GraphicalConsoleDto toConsoleDto(GraphicalConsole graphicalConsole) {
        if (graphicalConsole == null) {
            return null;
        }

        return GraphicalConsoleDto
            .newBuilder()
            .serviceEnabled(graphicalConsole.isServiceEnabled())
            .maxConcurrentSessions(graphicalConsole.getMaxConcurrentSessions())
            .connectTypesSupported(graphicalConsole.getConnectTypesSupported())
            .build();
    }

    public ConsoleDto toConsoleDto(SerialConsole serialConsole) {
        if (serialConsole == null) {
            return null;
        }

        return ConsoleDto
            .newBuilder()
            .serviceEnabled(serialConsole.isServiceEnabled())
            .maxConcurrentSessions(serialConsole.getMaxConcurrentSessions())
            .connectTypesSupported(serialConsole.getConnectTypesSupported())
            .build();
    }

    public ConsoleDto toConsoleDto(CommandShell commandShell) {
        if (commandShell == null) {
            return null;
        }

        return ConsoleDto
            .newBuilder()
            .serviceEnabled(commandShell.isServiceEnabled())
            .maxConcurrentSessions(commandShell.getMaxConcurrentSessions())
            .connectTypesSupported(commandShell.getConnectTypesSupported())
            .build();
    }

    private NetworkProtocolDto mapNetworkProtocol(NetworkProtocol networkProtocol) {
        if (networkProtocol == null) {
            return null;
        }
        return NetworkProtocolDto.newBuilder()
            .id(networkProtocol.getId().toString())
            .name(networkProtocol.getName())
            .description(networkProtocol.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(networkProtocol.getService(), networkProtocol.getUnknownOems()))
            .status(networkProtocol.getStatus())
            .hostName(networkProtocol.getHostName())
            .fqdn(networkProtocol.getFqdn())
            .http(toProtocolDto(networkProtocol.getHttpProtocolEnabled(), networkProtocol.getHttpPort()))
            .https(toProtocolDto(networkProtocol.getHttpsProtocolEnabled(), networkProtocol.getHttpsPort()))
            .ipmi(toProtocolDto(networkProtocol.getIpmiProtocolEnabled(), networkProtocol.getIpmiPort()))
            .ssh(toProtocolDto(networkProtocol.getSshProtocolEnabled(), networkProtocol.getSshPort()))
            .snmp(toProtocolDto(networkProtocol.getSnmpProtocolEnabled(), networkProtocol.getSnmpPort()))
            .virtualMedia(toProtocolDto(networkProtocol.getVirtualMediaProtocolEnabled(), networkProtocol.getVirtualMediaPort()))
            .ssdp(getSsdpProtocolDto(networkProtocol))
            .telnet(toProtocolDto(networkProtocol.getTelnetProtocolEnabled(), networkProtocol.getTelnetPort()))
            .kvmip(toProtocolDto(networkProtocol.getKvmIpProtocolEnabled(), networkProtocol.getKvmIpPort()))
            .build();
    }

    private SimpleServiceDiscoveryProtocolDto getSsdpProtocolDto(NetworkProtocol networkProtocol) {
        return SimpleServiceDiscoveryProtocolDto
            .newBuilder()
            .protocolEnabled(networkProtocol.getSsdpProtocolEnabled())
            .port(networkProtocol.getSsdpPort())
            .notifyMulticastIntervalSeconds(networkProtocol.getSsdpNotifyMulticastIntervalSeconds())
            .notifyTtl(networkProtocol.getSsdpNotifyTtl())
            .notifyIpV6Scope(networkProtocol.getSsdpNotifyIpV6Scope())
            .build();

    }

    private ProtocolDto toProtocolDto(Boolean protocolEnabled, Integer port) {
        return ProtocolDto
            .newBuilder()
            .protocolEnabled(protocolEnabled)
            .port(port)
            .build();
    }
}
