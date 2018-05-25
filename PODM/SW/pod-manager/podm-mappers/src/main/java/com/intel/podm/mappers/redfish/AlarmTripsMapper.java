/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.embeddables.AlarmTrips;
import com.intel.podm.client.resources.redfish.AlarmTripsObject;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class AlarmTripsMapper extends SubresourceMapper<AlarmTripsObject, AlarmTrips> {
    @Inject
    public AlarmTripsMapper(EmbeddableCleanAndCreateStrategy<AlarmTrips> embeddableCleanAndCreateStrategy) {
        super(AlarmTrips.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(AlarmTripsObject source, AlarmTrips target) {
        if (source == null) {
            return;
        }
        target.setAddressParityError(source.getAddressParityError());
        target.setSpareBlock(source.getSpareBlock());
        target.setTemperature(source.getTemperature());
        target.setUncorrectableEccError(source.getUncorrectableEccError());
        target.setCorrectableEccError(source.getCorrectableEccError());
    }

    @Override
    @SuppressWarnings("checkstyle:BooleanExpressionComplexity")
    public boolean equals(AlarmTripsObject source, AlarmTrips target) {
        return Objects.equals(source.getAddressParityError(), target.getAddressParityError())
            && Objects.equals(source.getSpareBlock(), target.getSpareBlock())
            && Objects.equals(source.getTemperature(), target.getTemperature())
            && Objects.equals(source.getUncorrectableEccError(), target.getUncorrectableEccError())
            && Objects.equals(source.getCorrectableEccError(), target.getCorrectableEccError());
    }
}
