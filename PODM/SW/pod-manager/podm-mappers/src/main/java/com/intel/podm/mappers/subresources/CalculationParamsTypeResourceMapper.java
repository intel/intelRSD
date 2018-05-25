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

import com.intel.podm.business.entities.redfish.embeddables.CalculationParamsType;
import com.intel.podm.client.resources.redfish.CalculationParamsTypeObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class CalculationParamsTypeResourceMapper extends SubresourceMapper<CalculationParamsTypeObject, CalculationParamsType> {
    @Inject
    public CalculationParamsTypeResourceMapper(EmbeddableCleanAndCreateStrategy<CalculationParamsType> embeddableCleanAndCreateStrategy) {
        super(CalculationParamsType.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(CalculationParamsTypeObject calculationParamsTypeObject, CalculationParamsType calculationParamsType) {
        calculationParamsType.setResultMetric(calculationParamsTypeObject.getResultMetric());
        calculationParamsType.setSourceMetric(calculationParamsTypeObject.getSourceMetric());
    }

    @Override
    protected boolean equals(CalculationParamsTypeObject calculationParamsTypeObject, CalculationParamsType calculationParamsType) {
        return Objects.equals(calculationParamsTypeObject.getResultMetric(), calculationParamsType.getResultMetric())
            && Objects.equals(calculationParamsTypeObject.getSourceMetric(), calculationParamsType.getSourceMetric());

    }
}
