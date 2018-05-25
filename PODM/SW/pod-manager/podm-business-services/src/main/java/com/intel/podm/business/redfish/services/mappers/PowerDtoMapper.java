/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.InputRangeDto;
import com.intel.podm.business.dto.PowerControlDto;
import com.intel.podm.business.dto.PowerDto;
import com.intel.podm.business.dto.PowerSupplyDto;
import com.intel.podm.business.dto.PowerVoltageDto;
import com.intel.podm.business.dto.RedundancyDto;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Power;
import com.intel.podm.business.entities.redfish.PowerControl;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerVoltage;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.embeddables.InputRange;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.OemSerializeHelper.oemDtoToJsonNode;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.util.stream.Collectors.toList;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class PowerDtoMapper extends DtoMapper<Power, PowerDto> {
    @Inject
    private PowerControlMapper powerControlMapper;

    @Inject
    private PowerVoltageMapper powerVoltageMapper;

    @Inject
    private PowerSupplyMapper powerSupplyMapper;

    @Inject
    private RedundancyDtoMapper redundancyDtoMapper;

    PowerDtoMapper() {
        super(Power.class, PowerDto.class);
        this.ignoredProperties("powerControls", "voltages", "powerSupplies", "redundancies");
    }

    @Override
    protected void performNotAutomatedMapping(Power source, PowerDto target) {
        super.performNotAutomatedMapping(source, target);

        mapPowerControls(source, target);
        mapPowerVoltages(source, target);
        mapPowerSupplies(source, target);
        mapRedundancies(source, target);
        mapOem(source, target);
    }

    private void mapRedundancies(Power source, PowerDto target) {
        Set<RedundancyDto> redundancies = target.getRedundancies() != null ? target.getRedundancies() : new TreeSet<>();

        for (Redundancy redundancy : source.getOwnedRedundancies()) {
            RedundancyDto redundancyDto = new RedundancyDto();
            redundancyDtoMapper.map(redundancy, redundancyDto);
            redundancies.add(redundancyDto);
        }
        target.setRedundancies(redundancies);
    }

    private void mapPowerSupplies(Power source, PowerDto target) {
        Set<PowerSupplyDto> powerSupplies = target.getPowerSupplies() != null ? target.getPowerSupplies() : new TreeSet<>();
        powerSupplyMapper.ignoredProperties("context", "redundancy", "relatedItem", "inputRanges");

        for (PowerSupply powerSupply : source.getPowerSupplies()) {
            PowerSupplyDto powerSupplyDto = new PowerSupplyDto();
            powerSupplyMapper.map(powerSupply, powerSupplyDto);
            powerSupplies.add(powerSupplyDto);
        }
        target.setPowerSupplies(powerSupplies);
    }

    private void mapPowerVoltages(Power source, PowerDto target) {
        Set<PowerVoltageDto> voltages = target.getVoltages() != null ? target.getVoltages() : new TreeSet<>();
        powerVoltageMapper.ignoredProperties("context", "relatedItem");

        for (PowerVoltage powerVoltage : source.getVoltages()) {
            PowerVoltageDto powerVoltageDto = new PowerVoltageDto();
            powerVoltageMapper.map(powerVoltage, powerVoltageDto);
            voltages.add(powerVoltageDto);
        }
        target.setVoltages(voltages);
    }

    private void mapPowerControls(Power source, PowerDto target) {
        Set<PowerControlDto> powerControls = target.getPowerControls() != null ? target.getPowerControls() : new TreeSet<>();
        powerControlMapper.ignoredProperties("context", "relatedItems");

        for (PowerControl powerControl : source.getPowerControls()) {
            PowerControlDto powerControlDto = new PowerControlDto();
            powerControlMapper.map(powerControl, powerControlDto);
            powerControls.add(powerControlDto);
        }
        target.setPowerControls(powerControls);
    }

    private void mapOem(Power source, PowerDto target) {
        PowerDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setInputAcPowerWatts(source.getInputAcPowerWatts());
    }

    @Dependent
    private static class PowerControlMapper extends BrilliantMapper<PowerControl, PowerControlDto> {
        @Inject
        private UnknownOemTranslator unknownOemTranslator;

        PowerControlMapper() {
            super(PowerControl.class, PowerControlDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(PowerControl source, PowerControlDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setoDataId(asOdataId(toContext(source)));
            target.setRelatedItems(source.getRelatedItems().stream()
                .map(Contexts::toContext).collect(toList()));
            target.setOem(oemDtoToJsonNode(unknownOemTranslator.translateStringOemToDto(source.getService(), source.getOem())));
        }
    }

    @Dependent
    private static class PowerVoltageMapper extends BrilliantMapper<PowerVoltage, PowerVoltageDto> {
        PowerVoltageMapper() {
            super(PowerVoltage.class, PowerVoltageDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(PowerVoltage source, PowerVoltageDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setoDataId(asOdataId(toContext(source)));
            target.setRelatedItem(source.getRelatedItems().stream()
                .map(Contexts::toContext)
                .collect(toList()));
        }
    }

    @Dependent
    private static class PowerSupplyMapper extends BrilliantMapper<PowerSupply, PowerSupplyDto> {
        @Inject
        private UnknownOemTranslator unknownOemTranslator;

        PowerSupplyMapper() {
            super(PowerSupply.class, PowerSupplyDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(PowerSupply source, PowerSupplyDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setOem(oemDtoToJsonNode(unknownOemTranslator.translateStringOemToDto(source.getService(), source.getOem())));
            target.setoDataId(asOdataId(toContext(source)));
            target.setRedundancy(source.getRedundancies().stream()
                .map(Contexts::toContext)
                .collect(toList()));
            target.setRelatedItem(source.getRelatedItems().stream()
                .map(Contexts::toContext)
                .collect(toList()));
            target.setInputRanges(mapInputRanges(source));
        }

        private List<InputRangeDto> mapInputRanges(PowerSupply powerSupply) {
            return powerSupply.getInputRanges().stream()
                .map(inputRange -> buildInputRangeDto(powerSupply.getService(), inputRange))
                .collect(toList());
        }

        private InputRangeDto buildInputRangeDto(ExternalService externalService, InputRange inputRange) {
            InputRangeDto dto = new InputRangeDto();
            dto.setInputType(inputRange.getInputType());
            dto.setMinimumVoltage(inputRange.getMinimumVoltage());
            dto.setMaximumVoltage(inputRange.getMaximumVoltage());
            dto.setMinimumFrequencyHz(inputRange.getMinimumFrequencyHz());
            dto.setMaximumFrequencyHz(inputRange.getMaximumFrequencyHz());
            dto.setOutputWattage(inputRange.getOutputWattage());
            dto.setOem(oemDtoToJsonNode(unknownOemTranslator.translateStringOemToDto(externalService, inputRange.getOem())));
            return dto;
        }
    }
}
