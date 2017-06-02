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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.ChassisDto;
import com.intel.podm.redfish.json.templates.ChassisJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.redfish.ResourceNames.POWER_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.THERMAL_RESOURCE_NAME;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

public final class ChassisDtoJsonSerializer extends BaseDtoJsonSerializer<ChassisDto> {
    public ChassisDtoJsonSerializer() {
        super(ChassisDto.class);
    }

    @Override
    protected ChassisJson translate(ChassisDto dto) {
        ChassisJson chassisJson = new ChassisJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        chassisJson.oDataId = oDataId;
        chassisJson.oDataContext = getContextFromId(oDataId);

        addLinks(chassisJson, dto.getLinks());
        addBasicInformation(chassisJson, dto);
        addOemSpecificInformation(chassisJson, dto);

        return chassisJson;
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount"})
    private void addBasicInformation(ChassisJson chassisJson, ChassisDto dto) {
        final ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        chassisJson.id = dto.getId();
        chassisJson.sku = dto.getSku();
        chassisJson.name = dto.getName();
        chassisJson.model = dto.getModel();
        chassisJson.assetTag = dto.getAssetTag();
        chassisJson.powerZones = oDataIdFromUri(create(oDataId + "/PowerZones"));
        chassisJson.partNumber = dto.getPartNumber();
        chassisJson.chassisType = dto.getChassisType();
        chassisJson.description = dto.getDescription();
        chassisJson.thermalZones = oDataIdFromUri(create(oDataId + "/ThermalZones"));
        chassisJson.manufacturer = dto.getManufacturer();
        chassisJson.serialNumber = dto.getSerialNumber();
        chassisJson.indicatorLed = dto.getIndicatorLed();
        chassisJson.status = dto.getStatus();
        if (dto.getThermalDto() != null) {
            chassisJson.thermal = oDataIdFromUri(create(oDataId + "/" + THERMAL_RESOURCE_NAME));
        }
        if (dto.getPowerDto() != null) {
            chassisJson.power = oDataIdFromUri(create(oDataId + "/" + POWER_RESOURCE_NAME));
        }
    }

    private static void addLinks(ChassisJson chassisJson, ChassisDto.Links dto) {
        chassisJson.links.containedBy = asOdataId(dto.getContainedBy());
        chassisJson.links.contains.addAll(asOdataIdSet(dto.getContains()));
        chassisJson.links.computerSystems.addAll(asOdataIdSet(dto.getComputerSystems()));
        chassisJson.links.managedBy.addAll(asOdataIdSet(dto.getManagedBy()));
        chassisJson.links.managersInChassis.addAll(asOdataIdSet(dto.getManagersInChassis()));
        chassisJson.links.drives.addAll(asOdataIdSet(dto.getDrives()));
        chassisJson.links.storage.addAll(asOdataIdSet(dto.getStorage()));
        chassisJson.links.oem.rackScaleOem.switches.addAll(asOdataIdSet(dto.getSwitches()));
    }

    private static void addOemSpecificInformation(ChassisJson chassisJson, ChassisDto dto) {
        if (RACK.equals(dto.getChassisType())) {
            ChassisJson.RackChassisOem rackChassisJson = chassisJson.createNewRackChassisOem();
            rackChassisJson.rackChassisOem.geoTag = dto.getGeoTag();
            rackChassisJson.rackChassisOem.rackSupportsDisaggregatedPowerCooling = dto.getRackSupportsDisaggregatedPowerCooling();
            rackChassisJson.rackChassisOem.rmmPresent = dto.getRmmPresent();
            rackChassisJson.rackChassisOem.uuid = dto.getUuid();
            rackChassisJson.rackChassisOem.location.id = dto.getLocationId();
            rackChassisJson.rackChassisOem.location.parentId = dto.getLocationParentId();
            chassisJson.oem = rackChassisJson;
        } else {
            ChassisJson.Oem chassisOem = chassisJson.createNewChassisOem();
            chassisOem.rackScaleOem.location.id = dto.getLocationId();
            chassisOem.rackScaleOem.location.parentId = dto.getLocationParentId();
            chassisJson.oem = chassisOem;
        }
    }
}
