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

import com.intel.podm.business.dto.MetricDefinitionDto;
import com.intel.podm.business.dto.MetricDefinitionDto.Oem.RackScaleOem;
import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.List;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.net.URI.create;
import static java.util.stream.Collectors.toList;

@Dependent
class MetricDefinitionDtoMapper extends DtoMapper<MetricDefinition, MetricDefinitionDto> {

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    protected MetricDefinitionDtoMapper() {
        super(MetricDefinition.class, MetricDefinitionDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(MetricDefinition source, MetricDefinitionDto target) {
        super.performNotAutomatedMapping(source, target);
        ExternalService externalService = source.getService();
        List<String> collect = source.getMetricProperties().stream()
            .map(uriValue -> transformUriToNorthboundFormat(externalService, uriValue))
            .collect(toList());
        target.setMetricProperties(collect);

        mapRackScaleOem(source, target);
    }

    private void mapRackScaleOem(MetricDefinition source, MetricDefinitionDto target) {
        RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();

        rackScaleOem.setCalculationPrecision(source.getCalculationPrecision());
        rackScaleOem.setDiscreteMetricType(source.getDiscreteMetricType());
    }

    private String transformUriToNorthboundFormat(ExternalService externalService, String southboundUriValue) {
        URI resourceUri = create(southboundUriValue);
        DiscoverableEntity entityFromSourceUri = discoverableEntityDao.findBy(externalService, create(resourceUri.getPath()), DiscoverableEntity.class);
        Context context = toContext(entityFromSourceUri);
        URI uri = context.asOdataId().toUri();
        return uri + "#" + resourceUri.getFragment();
    }
}
