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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.RedundancyDto;
import com.intel.podm.business.dto.ThermalDto;
import com.intel.podm.business.dto.ThermalDto.ThermalFanDto;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.Thermal;
import com.intel.podm.business.entities.redfish.ThermalFan;
import com.intel.podm.business.entities.redfish.ThermalTemperature;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Set;
import java.util.TreeSet;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class ThermalDtoMapper extends DtoMapper<Thermal, ThermalDto> {
    @Inject
    private ThermalTemperatureMapper thermalTemperatureMapper;

    @Inject
    private RedundancyDtoMapper redundancyDtoMapper;

    @Inject
    private ThermalFanMapper thermalFanMapper;

    ThermalDtoMapper() {
        super(Thermal.class, ThermalDto.class);
        this.ignoredProperties("temperatures", "fans", "redundancies");
    }

    @Override
    protected void performNotAutomatedMapping(Thermal source, ThermalDto target) {
        super.performNotAutomatedMapping(source, target);

        mapStatus(source, target);
        mapThermalTemperatures(source, target);
        mapThermalFans(source, target);
        mapRedundancies(source, target);
        mapOem(source, target);
    }

    private void mapStatus(Thermal source, ThermalDto target) {
        ofNullable(source.getStatus()).ifPresent(target::setStatus);
    }

    private void mapOem(Thermal source, ThermalDto target) {
        ThermalDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setDesiredSpeedPwm(source.getDesiredSpeedPwm());
        rackScaleOem.setVolumetricAirflowCfm(source.getVolumetricAirflowCfm());
    }

    private void mapThermalTemperatures(Thermal source, ThermalDto target) {
        Set<ThermalDto.ThermalTemperatureDto> temperatures = target.getTemperatures() != null ? target.getTemperatures() : new TreeSet<>();

        for (ThermalTemperature thermalTemperature : source.getTemperatures()) {
            ThermalDto.ThermalTemperatureDto thermalTemperatureDto = new ThermalDto.ThermalTemperatureDto();
            thermalTemperatureMapper.map(thermalTemperature, thermalTemperatureDto);
            temperatures.add(thermalTemperatureDto);
        }
        target.setTemperatures(temperatures);
    }

    private void mapThermalFans(Thermal source, ThermalDto target) {
        Set<ThermalFanDto> fans = target.getFans() != null ? target.getFans() : new TreeSet<>();

        for (ThermalFan thermalFan : source.getFans()) {
            ThermalFanDto thermalFanDto = new ThermalFanDto();
            thermalFanMapper.map(thermalFan, thermalFanDto);
            fans.add(thermalFanDto);
        }
        target.setFans(fans);
    }

    private void mapRedundancies(Thermal source, ThermalDto target) {
        Set<RedundancyDto> redundancies = target.getRedundancies() != null ? target.getRedundancies() : new TreeSet<>();

        for (Redundancy redundancy : source.getOwnedRedundancies()) {
            RedundancyDto redundancyDto = new RedundancyDto();
            redundancyDtoMapper.map(redundancy, redundancyDto);
            redundancies.add(redundancyDto);
        }
        target.setRedundancies(redundancies);
    }

    @Dependent
    private static class ThermalFanMapper extends BrilliantMapper<ThermalFan, ThermalFanDto> {
        ThermalFanMapper() {
            super(ThermalFan.class, ThermalFanDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(ThermalFan source, ThermalFanDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setOdataId(asOdataId(toContext(source)));
            target.setRedundancy(source.getRedundancies().stream()
                .map(Contexts::toContext)
                .collect(toSet()));
            target.setRelatedItem(source.getRelatedItems().stream()
                .map(Contexts::toContext)
                .collect(toSet()));
        }
    }

    @Dependent
    private static class ThermalTemperatureMapper extends BrilliantMapper<ThermalTemperature, ThermalDto.ThermalTemperatureDto> {
        ThermalTemperatureMapper() {
            super(ThermalTemperature.class, ThermalDto.ThermalTemperatureDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(ThermalTemperature source, ThermalDto.ThermalTemperatureDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setOdataId(asOdataId(toContext(source)));
            target.setRelatedItem(source.getRelatedItems().stream()
                .map(Contexts::toContext)
                .collect(toSet()));
        }
    }
}
