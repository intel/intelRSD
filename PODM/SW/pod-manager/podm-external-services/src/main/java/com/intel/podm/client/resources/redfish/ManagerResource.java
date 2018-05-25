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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.List;
import java.util.UUID;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static java.util.Collections.emptyList;

@OdataTypes({
    "#Manager" + VERSION_PATTERN + "Manager"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ManagerResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("UUID")
    private UUID uuid;

    @JsonProperty("ManagerType")
    private ManagerType managerType;

    @JsonProperty("ServiceEntryPointUUID")
    private UUID serviceEntryPointUuid;

    @JsonProperty("DateTime")
    private String dateTime;

    @JsonProperty("DateTimeLocalOffset")
    private String dateTimeLocalOffset;

    @JsonProperty("Model")
    private String model;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("PowerState")
    private PowerState powerState;

    @JsonProperty("GraphicalConsole")
    private GraphicalConsoleObject graphicalConsole;

    @JsonProperty("SerialConsole")
    private ConsoleObject serialConsole;

    @JsonProperty("CommandShell")
    private ConsoleObject commandShell;

    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;

    @JsonProperty("NetworkProtocol")
    private ODataId networkProtocol;

    @JsonProperty("EthernetInterfaces")
    private ODataId ethernetInterfaces;

    @JsonProperty("Links")
    private Links links = new Links();

    public UUID getUuid() {
        return uuid;
    }

    public ManagerType getManagerType() {
        return managerType;
    }

    public UUID getServiceEntryPointUuid() {
        return serviceEntryPointUuid;
    }

    public String getModel() {
        return model;
    }

    public String getDateTimeLocalOffset() {
        return dateTimeLocalOffset;
    }

    public String getDateTime() {
        return dateTime;
    }

    public Status getStatus() {
        return status;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public GraphicalConsoleObject getGraphicalConsole() {
        return graphicalConsole;
    }

    public ConsoleObject getSerialConsole() {
        return serialConsole;
    }

    public ConsoleObject getCommandShell() {
        return commandShell;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    @LinkName("networkProtocol")
    public ResourceSupplier getNetworkProtocol() {
        if (networkProtocol == null) {
            return null;
        }

        return toSupplier(networkProtocol);
    }

    @LinkName("ethernetInterfaces")
    public Iterable<ResourceSupplier> getEthernetInterfaces() throws WebClientRequestException {
        return processMembersListResource(ethernetInterfaces);
    }

    @LinkName("managedComputerSystems")
    public Iterable<ResourceSupplier> getManagedComputerSystems() throws WebClientRequestException {
        return toSuppliers(links.managerForServers);
    }

    @LinkName("managedChassisCollection")
    public Iterable<ResourceSupplier> getManagedChassisCollection() throws WebClientRequestException {
        return toSuppliers(links.managerForChassis);
    }

    @LinkName("managerInChassis")
    public ResourceSupplier getManagerInChassis() {
        if (links.managerInChassis == null) {
            return null;
        }
        return toSupplier(links.managerInChassis);
    }

    @LinkName("managedEthernetSwitches")
    public Iterable<ResourceSupplier> getManagedEthernetSwitches() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.managerForSwitches);
    }

    @LinkName("managedServices")
    public Iterable<ResourceSupplier> getManagedServices() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.managerForServices);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("ManagerForServers")
        private List<ODataId> managerForServers;

        @JsonProperty("ManagerForChassis")
        private List<ODataId> managerForChassis;

        @JsonProperty("ManagerInChassis")
        private ODataId managerInChassis;

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public class RackScaleOem {
                @JsonProperty("ManagerForSwitches")
                private List<ODataId> managerForSwitches;

                @JsonProperty("ManagerForServices")
                private List<ODataId> managerForServices = emptyList();
            }
        }
    }
}
