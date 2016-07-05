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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.ChassisDto;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.redfish.json.templates.ChassisJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.discovery.internal.PodStartupDiscovery.DEFAULT_POD_LOCATION_COORDINATE;
import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdFromContext;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdsCollection;
import static java.net.URI.create;

public final class ChassisDtoJsonSerializer extends DtoJsonSerializer<ChassisDto> {
    public ChassisDtoJsonSerializer() {
        super(ChassisDto.class);
    }

    @Override
    protected ChassisJson translate(ChassisDto dto) {

        ChassisJson chassisJson = new ChassisJson();

        ODataId oDataId = oDataId(context.getRequestPath());
        chassisJson.oDataId = oDataId;
        chassisJson.oDataContext = getContextFromId(oDataId);

        addLinks(chassisJson, dto);
        addBasicInformation(chassisJson, dto);
        addOemSpecificInformation(chassisJson, dto);

        return chassisJson;
    }

    private void addBasicInformation(ChassisJson chassisJson, ChassisDto dto) {
        final ODataId oDataId = ODataId.oDataId(context.getRequestPath());

        chassisJson.id = dto.getId();
        chassisJson.sku = dto.getSku();
        chassisJson.name = dto.getName();
        chassisJson.model = dto.getModel();
        chassisJson.assetTag = dto.getAssetTag();
        chassisJson.powerZones = oDataId(create(oDataId + "/PowerZones"));
        chassisJson.partNumber = dto.getPartNumber();
        chassisJson.chassisType = dto.getChassisType();
        chassisJson.description = dto.getDescription();
        chassisJson.thermalZones = oDataId(create(oDataId + "/ThermalZones"));
        chassisJson.manufacturer = dto.getManufacturer();
        chassisJson.serialNumber = dto.getSerialNumber();
        chassisJson.indicatorLed = dto.getIndicatorLed();
        chassisJson.status = dto.getStatus();
    }

    private static void addLinks(ChassisJson chassisJson, ChassisDto dto) {
        chassisJson.links.containedBy = oDataIdFromContext(dto.getContainedBy());
        chassisJson.links.contains.addAll(oDataIdsCollection(dto.getContains()));
        chassisJson.links.computerSystems.addAll(oDataIdsCollection(dto.getComputerSystems()));
        chassisJson.links.switches.addAll(oDataIdsCollection(dto.getSwitches()));
        chassisJson.links.managedBy.addAll(oDataIdsCollection(dto.getManagedBy()));
        chassisJson.links.managersInChassis.addAll(oDataIdsCollection(dto.getManagersInChassis()));
    }

    private static void addOemSpecificInformation(ChassisJson chassisJson, ChassisDto dto) {
        if (ChassisType.RACK.equals(dto.getChassisType())) {
            ChassisJson.RackChassisOem rackChassisJson = new ChassisJson.RackChassisOem();
            rackChassisJson.rackChassisOem.geoTag = dto.getGeoTag();
            rackChassisJson.rackChassisOem.rackSupportsDisaggregatedPowerCooling = dto.getRackSupportsDisaggregatedPowerCooling();
            rackChassisJson.rackChassisOem.rmmPresent = dto.getRmmPresent();
            rackChassisJson.rackChassisOem.uuid = dto.getUuid();
            rackChassisJson.rackChassisOem.location.id = dto.getLocationId();
            rackChassisJson.rackChassisOem.location.parentId = DEFAULT_POD_LOCATION_COORDINATE;
            chassisJson.oem = rackChassisJson;
        } else {
            ChassisJson.ChassisOem chassisOem = new ChassisJson.ChassisOem();
            chassisOem.rackScaleOem.location.id = dto.getLocationId();
            chassisOem.rackScaleOem.location.parentId = dto.getLocationParentId();
            chassisJson.oem = chassisOem;
        }
    }
}
