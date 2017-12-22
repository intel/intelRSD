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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.embeddables.HealthData;
import com.intel.podm.client.resources.redfish.HealthDataObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.podm.mappers.subresources.SubresourceMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class HealthDataMapper extends SubresourceMapper<HealthDataObject, HealthData> {
    @Inject
    private AlarmTripsMapper alarmTripsMapper;

    @Inject
    public HealthDataMapper(EmbeddableCleanAndCreateStrategy<HealthData> embeddableCleanAndCreateStrategy) {
        super(HealthData.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(HealthDataObject source, HealthData target) {
        if (source == null) {
            return;
        }
        target.setDataLossDetected(source.getDataLossDetected());
        target.setLastShutdownSuccess(source.getLastShutdownSuccess());
        target.setPerformanceDegraded(source.getPerformanceDegraded());
        target.setRemainingSpareBlockPercentage(source.getRemainingSpareBlockPercentage());
        alarmTripsMapper.map(source.getAlarmTrips(), target.getAlarmTrips());
    }

    @Override
    @SuppressWarnings("checkstyle:BooleanExpressionComplexity")
    public boolean equals(HealthDataObject source, HealthData target) {
        return Objects.equals(source.getDataLossDetected(), target.getDataLossDetected())
            && Objects.equals(source.getLastShutdownSuccess(), target.getLastShutdownSuccess())
            && Objects.equals(source.getPerformanceDegraded(), target.getPerformanceDegraded())
            && Objects.equals(source.getRemainingSpareBlockPercentage(), target.getRemainingSpareBlockPercentage())
            && alarmTripsMapper.equals(source.getAlarmTrips(), target.getAlarmTrips());
    }
}
