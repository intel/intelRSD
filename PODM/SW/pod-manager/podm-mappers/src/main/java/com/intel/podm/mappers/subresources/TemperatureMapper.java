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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.Temperature;
import com.intel.podm.client.api.resources.redfish.TemperatureResource;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class TemperatureMapper extends SubresourceMapper<TemperatureResource, Temperature> {
    @Inject
    public TemperatureMapper(EntityCleanAndCreateStrategy<Temperature> entityCleanAndCreateStrategy) {
        super(Temperature.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(TemperatureResource sourceTemperature, Temperature targetTemperature) {
        targetTemperature.setName(sourceTemperature.getName());
        targetTemperature.setReadingCelsius(sourceTemperature.getReadingCelsius());
        targetTemperature.setStatus(sourceTemperature.getStatus());
        targetTemperature.setPhysicalContext(sourceTemperature.getPhysicalContext());
    }

    @Override
    public boolean equals(TemperatureResource sourceTemperature, Temperature targetTemperature) {
        return Objects.equals(sourceTemperature.getName(), targetTemperature.getName())
                && Objects.equals(sourceTemperature.getStatus(), targetTemperature.getStatus())
                && Objects.equals(sourceTemperature.getReadingCelsius(), targetTemperature.getReadingCelsius())
                && Objects.equals(sourceTemperature.getPhysicalContext(), targetTemperature.getPhysicalContext());
    }
}
