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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.MetricReportDefinitionDto;
import com.intel.podm.business.entities.redfish.MetricItem;
import com.intel.podm.business.entities.redfish.MetricReportDefinition;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
class MetricReportDefinitionDtoMapper extends DtoMapper<MetricReportDefinition, MetricReportDefinitionDto> {
    protected MetricReportDefinitionDtoMapper() {
        super(MetricReportDefinition.class, MetricReportDefinitionDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(MetricReportDefinition source, MetricReportDefinitionDto target) {
        super.performNotAutomatedMapping(source, target);

        mapMetrics(source, target);
    }

    private void mapMetrics(MetricReportDefinition source, MetricReportDefinitionDto target) {
        for (MetricItem metricItem : source.getMetricItems()) {
            MetricReportDefinitionDto.MetricItemDto dto = new MetricReportDefinitionDto.MetricItemDto();
            dto.setMetricValue(metricItem.getMetricValue());
            if (metricItem.getMetricDefinition() != null) {
                dto.setMetricDefinition(getMetricDefinition(metricItem));
            }
            target.getLinks().getMetrics().add(dto);
        }
    }

    private Context getMetricDefinition(MetricItem metricItem) {
        return toContext(metricItem.getMetricDefinition());
    }
}
