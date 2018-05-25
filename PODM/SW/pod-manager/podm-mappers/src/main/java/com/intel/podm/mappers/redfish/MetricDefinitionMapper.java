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

import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.client.resources.redfish.MetricDefinitionResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.CalculationParamsTypeResourceMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import com.intel.podm.mappers.subresources.WildcardMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class MetricDefinitionMapper extends EntityMapper<MetricDefinitionResource, MetricDefinition> {

    @Inject
    SimpleTypeMapper simpleTypeMapper;

    @Inject
    WildcardMapper wildcardMapper;

    @Inject
    CalculationParamsTypeResourceMapper calculationParamsTypeResourceMapper;

    public MetricDefinitionMapper() {
        super(MetricDefinitionResource.class, MetricDefinition.class);
    }

    @Override
    protected void performNotAutomatedMapping(MetricDefinitionResource source, MetricDefinition target) {
        super.performNotAutomatedMapping(this.source, this.target);
        simpleTypeMapper.map(
            source.getMetricProperties(),
            target.getMetricProperties(),
            target::addMetricProperties);
        simpleTypeMapper.map(
            source.getDiscreteValues(),
            target.getDiscreteValues(),
            target::addDiscreteValues);
        calculationParamsTypeResourceMapper.map(source.getCalculationParameters(),
            target.getCalculationParameters(),
            target::addCalculationParameters);
        wildcardMapper.map(source.getWildcards(),
            target.getWildcards(),
            target::addWildcards);
    }
}
