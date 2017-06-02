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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.ManagerDto;
import com.intel.podm.redfish.json.templates.ManagerJson;
import com.intel.podm.redfish.json.templates.ManagerJson.Console;
import com.intel.podm.redfish.json.templates.ManagerJson.GraphicalConsole;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_PROTOCOL_RESOURCE_NAME;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.oDataIdOfCollectionInContext;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.oDataIdOfResourceInContext;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class ManagerDtoJsonSerializer extends BaseDtoJsonSerializer<ManagerDto> {
    public ManagerDtoJsonSerializer() {
        super(ManagerDto.class);
    }

    @Override
    protected ManagerJson translate(ManagerDto dto) {
        ManagerJson managerJson = new ManagerJson();
        mapBaseRedfishData(dto, managerJson);
        managerJson.managerType = dto.getManagerType();
        managerJson.serviceEntryPointUuid = dto.getServiceEntryPointUuid();
        managerJson.uuid = dto.getUuid();
        managerJson.model = dto.getModel();
        managerJson.status = dto.getStatus();
        managerJson.powerState = dto.getPowerState();
        managerJson.firmwareVersion = dto.getFirmwareVersion();
        fillConsoles(managerJson, dto);
        fillLinks(managerJson, dto.getLinks());

        if (dto.getNetworkProtocol() != null) {
            managerJson.networkProtocol = oDataIdOfResourceInContext(dto.getContext(), NETWORK_PROTOCOL_RESOURCE_NAME);
        }
        managerJson.ethernetInterfaces = oDataIdOfCollectionInContext(dto.getContext(), ETHERNET_INTERFACE);
        return managerJson;
    }

    private void mapBaseRedfishData(ManagerDto dto, ManagerJson managerJson) {
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        managerJson.oDataId = oDataId;
        managerJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        managerJson.id = dto.getId();
        managerJson.name = dto.getName();
        managerJson.description = dto.getDescription();
    }

    private void fillConsoles(ManagerJson managerJson, ManagerDto managerDto) {
        if (managerDto.getGraphicalConsole() != null) {
            managerJson.graphicalConsole = new GraphicalConsole();
            managerJson.graphicalConsole.serviceEnabled = managerDto.getGraphicalConsole().isServiceEnabled();
            managerJson.graphicalConsole.maxConcurrentSessions = managerDto.getGraphicalConsole().getMaxConcurrentSessions();
            managerJson.graphicalConsole.connectTypesSupported.addAll(managerDto.getGraphicalConsole().getConnectTypesSupported());
        }

        if (managerDto.getSerialConsole() != null) {
            managerJson.serialConsole = new Console();
            managerJson.serialConsole.serviceEnabled = managerDto.getSerialConsole().isServiceEnabled();
            managerJson.serialConsole.maxConcurrentSessions = managerDto.getSerialConsole().getMaxConcurrentSessions();
            managerJson.serialConsole.connectTypesSupported.addAll(managerDto.getSerialConsole().getConnectTypesSupported());
        }

        if (managerDto.getCommandShell() != null) {
            managerJson.commandShell = new Console();
            managerJson.commandShell.serviceEnabled = managerDto.getCommandShell().isServiceEnabled();
            managerJson.commandShell.maxConcurrentSessions = managerDto.getCommandShell().getMaxConcurrentSessions();
            managerJson.commandShell.connectTypesSupported.addAll(managerDto.getCommandShell().getConnectTypesSupported());
        }
    }

    private void fillLinks(ManagerJson managerJson, ManagerDto.Links links) {
        managerJson.links.oDataType = "#Manager.v1_1_0.Links";
        managerJson.links.managedComputerSystems.addAll(asOdataIdSet(links.getManagedComputerSystems()));
        managerJson.links.managedChassisCollection.addAll(asOdataIdSet(links.getManagedChassisCollection()));
        managerJson.links.managerInChassis = asOdataId(links.getManagerInChassis());
        managerJson.links.oem.rackScaleOem.managedEthernetSwitches.addAll(asOdataIdSet(links.getManagedEthernetSwitches()));
        managerJson.links.oem.rackScaleOem.oDataType = "#Intel.Oem.ManagerLinks";
        managerJson.links.oem.rackScaleOem.managerForServices.addAll(asOdataIdSet(links.getManagedServices()));
    }
}
