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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.Console;
import com.intel.podm.business.entities.redfish.properties.GraphicalConsole;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.business.entities.base.DomainObjectLink.COMMAND_SHELL;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.GRAPHIC_CONSOLE;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGER_IN_CHASSIS;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGES;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.SERIAL_CONSOLE;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.uuidProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class Manager extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<ManagerType> MANAGER_TYPE = enumProperty("managerType", ManagerType.class);
    public static final DomainObjectProperty<UUID> SERVICE_ENTRY_POINT_UUID = uuidProperty("serviceEntryPointUuid");
    public static final DomainObjectProperty<UUID> UUID = uuidProperty("uuid");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> FIRMWARE_VERSION = stringProperty("firmwareVersion");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public ManagerType getManagerType() {
        return getProperty(MANAGER_TYPE);
    }

    public void setManagerType(ManagerType type) {
        setProperty(MANAGER_TYPE, type);
    }

    public UUID getServiceEntryPointUuid() {
        return getProperty(SERVICE_ENTRY_POINT_UUID);
    }

    public void setServiceEntryPointUuid(UUID serviceEntryPointUuid) {
        setProperty(SERVICE_ENTRY_POINT_UUID, serviceEntryPointUuid);
    }

    public UUID getUuid() {
        return getProperty(UUID);
    }

    public void setUuid(UUID uuid) {
        setProperty(UUID, uuid);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public GraphicalConsole getGraphicalConsole() {
        return singleOrNull(getLinked(GRAPHIC_CONSOLE));
    }

    public void setGraphicalConsole(GraphicalConsole graphicalConsole) {
        link(GRAPHIC_CONSOLE, graphicalConsole);
    }

    public Console getSerialConsole() {
        return singleOrNull(getLinked(SERIAL_CONSOLE));
    }

    public void setSerialConsole(Console serialConsole) {
        link(SERIAL_CONSOLE, serialConsole);
    }

    public Console getCommandShell() {
        return singleOrNull(getLinked(COMMAND_SHELL));
    }

    public void setCommandShell(Console commandShell) {
        link(COMMAND_SHELL, commandShell);
    }

    public String getFirmwareVersion() {
        return getProperty(FIRMWARE_VERSION);
    }

    public void setFirmwareVersion(String firmwareVersion) {
        setProperty(FIRMWARE_VERSION, firmwareVersion);
    }

    public NetworkProtocol getNetworkProtocol() {
        //TODO: single() ?
        return singleOrNull(getLinked(CONTAINS, NetworkProtocol.class));
    }

    public void setNetworkProtocol(NetworkProtocol networkProtocol) {
        link(CONTAINS, networkProtocol);
    }

    public Collection<EthernetInterface> getEthernetInterfaces() {
        return getLinked(CONTAINS, EthernetInterface.class);
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        link(CONTAINS, ethernetInterface);
    }

    public Collection<ComputerSystem> getManagedComputerSystems() {
        return getLinked(MANAGES, ComputerSystem.class);
    }

    public void addComputerSystem(ComputerSystem computerSystem) {
        link(MANAGES, computerSystem);
    }

    public Collection<Chassis> getManagedChassisCollection() {
        return getLinked(MANAGES, Chassis.class);
    }

    public void addChassis(Chassis chassis) {
        link(MANAGES, chassis);
    }

    public Chassis getManagerInChassis() {
        return singleOrNull(getLinked(MANAGER_IN_CHASSIS, Chassis.class));
    }

    public void setManagerInChassis(Chassis chassis) {
        link(MANAGER_IN_CHASSIS, chassis);
    }

    public Collection<EthernetSwitch> getManagedEthernetSwitches() {
        return getLinked(MANAGES, EthernetSwitch.class);
    }

    public void addEthernetSwitch(EthernetSwitch ethernetSwitch) {
        link(MANAGES, ethernetSwitch);
    }

    public Collection<StorageService> getManagedServices() {
        return getLinked(MANAGES, StorageService.class);
    }

    public void addStorageService(StorageService storageService) {
        link(MANAGES, storageService);
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }
}
