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

import com.intel.podm.business.dto.redfish.ThermalDto;
import com.intel.podm.business.dto.redfish.attributes.ThermalFanDto;
import com.intel.podm.business.dto.redfish.attributes.ThermalTemperatureDto;
import com.intel.podm.redfish.json.templates.ThermalJson;
import com.intel.podm.redfish.json.templates.ThermalJson.ThermalFanJson;
import com.intel.podm.redfish.json.templates.ThermalJson.ThermalTemperatureJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

import static com.intel.podm.redfish.serializers.RedundancyDtoJsonMapper.getRedundanciesCollection;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class ThermalDtoJsonSerializer extends BaseDtoJsonSerializer<ThermalDto> {
    protected ThermalDtoJsonSerializer() {
        super(ThermalDto.class);
    }

    @Override
    @SuppressWarnings({"checkstyle:MethodLength"})
    protected ThermalJson translate(ThermalDto dto) {
        ThermalJson thermalJson = new ThermalJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        thermalJson.oDataId = oDataId;
        thermalJson.oDataContext = getContextFromId(oDataId);
        thermalJson.id = dto.getId();
        thermalJson.name = dto.getName();
        thermalJson.description = dto.getDescription();
        thermalJson.temperatures.addAll(getTemperaturesCollection(dto.getTemperatures()));
        thermalJson.fans.addAll(getFansCollection(dto.getFans()));
        thermalJson.redundancy.addAll(getRedundanciesCollection(dto.getRedundancies()));

        return thermalJson;
    }

    private Collection<ThermalTemperatureJson> getTemperaturesCollection(List<ThermalTemperatureDto> thermalTemperatureDtos) {
        return thermalTemperatureDtos.stream()
            .map(temperatureDto -> toTemperature(temperatureDto, String.valueOf(thermalTemperatureDtos.indexOf(temperatureDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private Collection<ThermalFanJson> getFansCollection(List<ThermalFanDto> thermalFansDtos) {
        return thermalFansDtos.stream()
            .map(fanDto -> toFan(fanDto, String.valueOf(thermalFansDtos.indexOf(fanDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount"})
    private ThermalFanJson toFan(ThermalFanDto dto, String jsonPointerId) {
        ThermalFanJson json = new ThermalFanJson();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), jsonPointerId);
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.lowerThresholdCritical = dto.getLowerThresholdCritical();
        json.lowerThresholdFatal = dto.getLowerThresholdFatal();
        json.lowerThresholdNonCritical = dto.getLowerThresholdNonCritical();
        json.upperThresholdCritical = dto.getUpperThresholdCritical();
        json.upperThresholdFatal = dto.getUpperThresholdFatal();
        json.upperThresholdNonCritical = dto.getUpperThresholdNonCritical();
        json.maxReadingRange = dto.getMaxReadingRange();
        json.minReadingRange = dto.getMinReadingRange();
        json.physicalContext = dto.getPhysicalContext();
        json.reading = dto.getReading();
        json.readingUnits = dto.getReadingUnits();
        json.status = dto.getStatus();
        json.redundancy.addAll(asJsonPointerOdataIdSet(dto.getRedundancy(), jsonPointerId));
        json.relatedItem.addAll(asJsonPointerOdataIdSet(dto.getRelatedItem(), jsonPointerId));
        return json;
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount"})
    private static ThermalTemperatureJson toTemperature(ThermalTemperatureDto dto, String counter) {
        ThermalTemperatureJson json = new ThermalTemperatureJson();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), counter);
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.lowerThresholdCritical = dto.getLowerThresholdCritical();
        json.lowerThresholdFatal = dto.getLowerThresholdFatal();
        json.lowerThresholdNonCritical = dto.getLowerThresholdNonCritical();
        json.maxReadingRange = dto.getMaxReadingRange();
        json.minReadingRange = dto.getMinReadingRange();
        json.physicalContext = dto.getPhysicalContext();
        json.readingCelsius = dto.getReadingCelsius();
        json.relatedItem.addAll(asOdataIdSet(dto.getRelatedItem()));
        json.sensorNumber = dto.getSensorNumber();
        json.status = dto.getStatus();
        json.upperThresholdCritical = dto.getUpperThresholdCritical();
        json.upperThresholdFatal = dto.getUpperThresholdFatal();
        json.upperThresholdNonCritical = dto.getUpperThresholdNonCritical();

        return json;
    }
}
