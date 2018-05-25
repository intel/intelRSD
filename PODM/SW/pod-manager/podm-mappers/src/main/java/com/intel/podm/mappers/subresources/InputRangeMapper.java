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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.InputRange;
import com.intel.podm.client.resources.redfish.InputRangeObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class InputRangeMapper extends SubresourceMapper<InputRangeObject, InputRange> {
    @Inject
    public InputRangeMapper(EmbeddableCleanAndCreateStrategy<InputRange> embeddableCleanAndCreateStrategy) {
        super(InputRange.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(InputRangeObject sourceInputRange, InputRange targetInputRange) {
        targetInputRange.setInputType(sourceInputRange.getInputType());
        targetInputRange.setMinimumVoltage(sourceInputRange.getMinimumVoltage());
        targetInputRange.setMaximumVoltage(sourceInputRange.getMaximumVoltage());
        targetInputRange.setOutputWattage(sourceInputRange.getOutputWattage());
        targetInputRange.setMinimumFrequencyHz(sourceInputRange.getMinimumFrequencyHz());
        targetInputRange.setMaximumFrequencyHz(sourceInputRange.getMaximumFrequencyHz());
    }

    @Override
    public boolean equals(InputRangeObject sourceInputRange, InputRange targetInputRange) {
        return Objects.equals(sourceInputRange.getInputType(), targetInputRange.getInputType())
            && voltagesEqual(sourceInputRange, targetInputRange)
            && Objects.equals(sourceInputRange.getOutputWattage(), targetInputRange.getOutputWattage())
            && frequenciesEqual(sourceInputRange, targetInputRange);
    }

    private boolean voltagesEqual(InputRangeObject sourceInputRange, InputRange targetInputRange) {
        return Objects.equals(sourceInputRange.getMinimumVoltage(), targetInputRange.getMinimumVoltage())
            && Objects.equals(sourceInputRange.getMaximumVoltage(), targetInputRange.getMaximumVoltage());
    }

    private boolean frequenciesEqual(InputRangeObject sourceInputRange, InputRange targetInputRange) {
        return Objects.equals(sourceInputRange.getMinimumFrequencyHz(), targetInputRange.getMinimumFrequencyHz())
            && Objects.equals(sourceInputRange.getMaximumFrequencyHz(), targetInputRange.getMaximumFrequencyHz());
    }
}
