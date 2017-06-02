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

import com.intel.podm.business.dto.redfish.PowerControlDto;
import com.intel.podm.business.dto.redfish.PowerDto;
import com.intel.podm.business.dto.redfish.PowerSupplyDto;
import com.intel.podm.business.dto.redfish.PowerVoltageDto;
import com.intel.podm.business.dto.redfish.attributes.InputRangeDto;
import com.intel.podm.redfish.json.templates.PowerJson;
import com.intel.podm.redfish.json.templates.PowerJson.PowerControlJson;
import com.intel.podm.redfish.json.templates.PowerJson.PowerLimitJson;
import com.intel.podm.redfish.json.templates.PowerJson.PowerMetricsJson;
import com.intel.podm.redfish.json.templates.PowerJson.PowerSupplyJson;
import com.intel.podm.redfish.json.templates.PowerJson.PowerVoltageJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Collectors;

import static com.intel.podm.redfish.serializers.RedundancyDtoJsonMapper.getRedundanciesCollection;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataIdCollection;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static com.intel.podm.rest.representation.json.serializers.OemSerializeHelper.oemDtoToJsonNode;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public class PowerDtoJsonSerializer extends BaseDtoJsonSerializer<PowerDto> {
    protected PowerDtoJsonSerializer() {
        super(PowerDto.class);
    }

    @Override
    protected PowerJson translate(PowerDto dto) {
        PowerJson powerJson = new PowerJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        powerJson.oDataId = oDataId;
        powerJson.oDataContext = getContextFromId(oDataId);
        powerJson.id = dto.getId();
        powerJson.name = dto.getName();
        powerJson.description = dto.getDescription();
        powerJson.powerControl.addAll(getPowerControlCollection(dto.getPowerControls()));
        powerJson.voltages.addAll(getPowerVoltagesCollection(dto.getVoltages()));
        powerJson.powerSupplies.addAll(getPowerSuppliesCollection(dto.getPowerSupplies()));
        powerJson.redundancy.addAll(getRedundanciesCollection(dto.getRedundancies()));

        return powerJson;
    }

    private Collection<PowerVoltageJson> getPowerVoltagesCollection(List<PowerVoltageDto> powerVoltageDtos) {
        return powerVoltageDtos.stream()
            .map(powerVoltageDto -> toVoltage(powerVoltageDto, String.valueOf(powerVoltageDtos.indexOf(powerVoltageDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private Collection<PowerSupplyJson> getPowerSuppliesCollection(List<PowerSupplyDto> powerSupplyDtos) {
        return powerSupplyDtos.stream()
            .map(powerSupplyDto -> toPowerSupply(powerSupplyDto, String.valueOf(powerSupplyDtos.indexOf(powerSupplyDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private Collection<PowerControlJson> getPowerControlCollection(List<PowerControlDto> powerControlDtos) {
        return powerControlDtos.stream()
            .map(powerControlDto -> toPowerControl(powerControlDto, String.valueOf(powerControlDtos.indexOf(powerControlDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private PowerVoltageJson toVoltage(PowerVoltageDto dto, String jsonPointerId) {
        PowerVoltageJson json = new PowerVoltageJson();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), jsonPointerId);
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.sensorNumber = dto.getSensorNumber();
        json.status = dto.getStatus();
        json.readingVolts = dto.getReadingVolts();
        json.upperThresholdNonCritical = dto.getUpperThresholdNonCritical();
        json.upperThresholdCritical = dto.getUpperThresholdCritical();
        json.upperThresholdFatal = dto.getUpperThresholdFatal();
        json.lowerThresholdNonCritical = dto.getLowerThresholdNonCritical();
        json.lowerThresholdCritical = dto.getLowerThresholdCritical();
        json.lowerThresholdFatal = dto.getLowerThresholdFatal();
        json.minReadingRange = dto.getMinReadingRange();
        json.maxReadingRange = dto.getMaxReadingRange();
        json.physicalContext = dto.getPhysicalContext();
        json.relatedItem.addAll(asJsonPointerOdataIdCollection(dto.getRelatedItem(), jsonPointerId));
        return json;
    }

    private PowerSupplyJson toPowerSupply(PowerSupplyDto dto, String jsonPointerId) {
        PowerSupplyJson json = new PowerSupplyJson();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), jsonPointerId);
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.status = dto.getStatus();
        json.powerSupplyType = dto.getPowerSupplyType();
        json.lineInputVoltageType = dto.getLineInputVoltageType();
        json.lineInputVoltage = dto.getLineInputVoltage();
        json.powerCapacityWatts = dto.getPowerCapacityWatts();
        json.lastPowerOutputWatts = dto.getLastPowerOutputWatts();
        json.model = dto.getModel();
        json.manufacturer = dto.getManufacturer();
        json.firmwareVersion = dto.getFirmwareVersion();
        json.serialNumber = dto.getSerialNumber();
        json.partNumber = dto.getPartNumber();
        json.sparePartNumber = dto.getSparePartNumber();
        json.inputRanges.addAll(dto.getInputRanges().stream().map(toInputRange()).collect(toList()));
        json.indicatorLed = dto.getIndicatorLed();
        json.redundancy.addAll(asJsonPointerOdataIdCollection(dto.getRedundancy(), jsonPointerId));
        json.relatedItem.addAll(asJsonPointerOdataIdCollection(dto.getRelatedItem(), jsonPointerId));
        json.oem = oemDtoToJsonNode(dto.getOem());
        return json;
    }

    private Function<InputRangeDto, PowerJson.InputRangeJson> toInputRange() {
        return dto -> {
            PowerJson.InputRangeJson json = new PowerJson.InputRangeJson();
            json.inputType = dto.getInputType();
            json.minimumVoltage = dto.getMinimumVoltage();
            json.maximumVoltage = dto.getMaximumVoltage();
            json.minimumFrequencyHz = dto.getMinimumFrequencyHz();
            json.maximumFrequencyHz = dto.getMaximumFrequencyHz();
            json.outputWattage = dto.getOutputWattage();
            json.oem = oemDtoToJsonNode(dto.getOem());
            return json;
        };
    }

    private PowerControlJson toPowerControl(PowerControlDto dto, String jsonPointerId) {
        PowerControlJson json = new PowerControlJson();
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), jsonPointerId);
        json.powerAllocatedWatts = dto.getPowerAllocatedWatts();
        json.powerAvailableWatts = dto.getPowerAvailableWatts();
        json.powerCapacityWatts = dto.getPowerCapacityWatts();
        json.powerRequestedWatts = dto.getPowerRequestedWatts();
        json.powerConsumedWatts = dto.getPowerConsumedWatts();
        json.relatedItem.addAll(asJsonPointerOdataIdCollection(dto.getRelatedItems(), jsonPointerId));
        json.status = dto.getStatus();
        json.oem = oemDtoToJsonNode(dto.getOem());

        if (dto.getPowerLimitDto() != null) {
            json.powerLimit = toPowerLimit(dto.getPowerLimitDto());
        }

        if (dto.getPowerMetricsDto() != null) {
            json.powerMetrics = toPowerMetrics(dto.getPowerMetricsDto());
        }
        return json;
    }

    private PowerMetricsJson toPowerMetrics(PowerControlDto.PowerMetricsDto dto) {
        PowerMetricsJson json = new PowerMetricsJson();
        json.minConsumedWatts = dto.getMinConsumedWatts();
        json.maxConsumedWatts = dto.getMaxConsumedWatts();
        json.averageConsumedWatts = dto.getAverageConsumedWatts();
        json.intervalInMin = dto.getIntervalInMin();
        return json;
    }

    private PowerLimitJson toPowerLimit(PowerControlDto.PowerLimitDto dto) {
        PowerLimitJson json = new PowerLimitJson();
        json.correctionInMs = dto.getCorrectionInMs();
        json.limitException = dto.getLimitException();
        json.limitInWatts = dto.getLimitInWatts();
        return json;
    }
}
