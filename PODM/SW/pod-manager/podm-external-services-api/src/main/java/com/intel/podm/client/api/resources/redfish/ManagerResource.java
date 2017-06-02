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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;

import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface ManagerResource extends ExternalServiceResource {
    UUID getUuid();
    ManagerType getManagerType();
    UUID getServiceEntryPointUuid();
    String getModel();
    Status getStatus();
    GraphicalConsoleObject getGraphicalConsole();
    ConsoleObject getSerialConsole();
    ConsoleObject getCommandShell();
    String getFirmwareVersion();
    PowerState getPowerState();
    ResourceSupplier getNetworkProtocol();
    Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getManagedComputerSystems() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getManagedChassisCollection() throws ExternalServiceApiReaderException;
    ResourceSupplier getManagerInChassis();
    Iterable<ResourceSupplier> getManagedEthernetSwitches() throws ExternalServiceApiReaderException;
    Iterable<ResourceSupplier> getManagedServices() throws ExternalServiceApiReaderException;
}
