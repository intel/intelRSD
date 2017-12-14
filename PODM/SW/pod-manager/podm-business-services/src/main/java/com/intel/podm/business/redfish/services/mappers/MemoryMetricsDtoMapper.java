/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.business.dto.MemoryMetricsDto;
import com.intel.podm.business.entities.redfish.MemoryMetrics;
import com.intel.podm.business.entities.redfish.embeddables.AlarmTrips;
import com.intel.podm.business.entities.redfish.embeddables.CurrentPeriod;
import com.intel.podm.business.entities.redfish.embeddables.HealthData;
import com.intel.podm.business.entities.redfish.embeddables.LifeTime;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.dto.MemoryMetricsDto.AlarmTripsDto;
import static com.intel.podm.business.dto.MemoryMetricsDto.CurrentPeriodDto;
import static com.intel.podm.business.dto.MemoryMetricsDto.HealthDataDto;
import static com.intel.podm.business.dto.MemoryMetricsDto.LifeTimeDto;

@Dependent
public class MemoryMetricsDtoMapper extends DtoMapper<MemoryMetrics, MemoryMetricsDto> {
    @Inject
    private HealthDataMapper healthDataMapper;

    MemoryMetricsDtoMapper() {
        super(MemoryMetrics.class, MemoryMetricsDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(MemoryMetrics source, MemoryMetricsDto target) {
        super.performNotAutomatedMapping(source, target);

        mapCurrentPeriod(source, target);
        mapLifeTime(source, target);
        if (source.getHealthData() != null) {
            healthDataMapper.map(source.getHealthData(), target.getHealthData());
        }
        mapMemoryMetricsOem(source, target);
    }

    private void mapMemoryMetricsOem(MemoryMetrics source, MemoryMetricsDto target) {
        MemoryMetricsDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setTemperatureCelsius(source.getTemperatureCelsius());
        rackScaleOem.setThermalMarginCelsius(source.getThermalMarginCelsius());
        rackScaleOem.setBandwidthPercent(source.getBandwidthPercent());
        rackScaleOem.setConsumedPowerWatt(source.getConsumedPowerWatt());
        rackScaleOem.setThrottledCyclesPercent(source.getThrottledCyclesPercent());
        rackScaleOem.setEccErrorsCount(source.getEccErrorsCount());
        rackScaleOem.setHealthList(source.getHealthList());
    }

    private void mapLifeTime(MemoryMetrics source, MemoryMetricsDto target) {
        LifeTimeDto lifeTimeDto = target.getLifeTime();
        LifeTime lifeTime = source.getLifeTime();
        if (lifeTime == null) {
            return;
        }
        lifeTimeDto.setBlocksRead(lifeTime.getBlocksRead());
        lifeTimeDto.setBlocksWritten(lifeTime.getBlocksWritten());
    }

    private void mapCurrentPeriod(MemoryMetrics source, MemoryMetricsDto target) {
        CurrentPeriodDto currentPeriodDto = target.getCurrentPeriod();
        CurrentPeriod currentPeriod = source.getCurrentPeriod();
        if (currentPeriod == null) {
            return;
        }
        currentPeriodDto.setBlocksRead(currentPeriod.getBlocksRead());
        currentPeriodDto.setBlocksWritten(currentPeriod.getBlocksWritten());
    }

    @Dependent
    private static class HealthDataMapper extends BrilliantMapper<HealthData, HealthDataDto> {
        HealthDataMapper() {
            super(HealthData.class, HealthDataDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(HealthData source, HealthDataDto target) {
            super.performNotAutomatedMapping(source, target);

            AlarmTripsDto alarmTripsDto = target.getAlarmTrips();
            AlarmTrips alarmTrips = source.getAlarmTrips();
            if (alarmTrips == null) {
                return;
            }
            alarmTripsDto.setAddressParityError(alarmTrips.getAddressParityError());
            alarmTripsDto.setCorrectableEccError(alarmTrips.getCorrectableEccError());
            alarmTripsDto.setSpareBlock(alarmTrips.getSpareBlock());
            alarmTripsDto.setTemperature(alarmTrips.getTemperature());
            alarmTripsDto.setUncorrectableEccError(alarmTrips.getUncorrectableEccError());
        }
    }

}
