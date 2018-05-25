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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.MetricDefinitionDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.MetricDefinitionDao;
import com.intel.podm.business.entities.redfish.MetricDefinition;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.TELEMETRY_SERVICE;
import static com.intel.podm.common.types.Id.id;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
public class MetricDefinitionServiceImpl implements ReaderService<MetricDefinitionDto> {
    @Inject
    private MetricDefinitionDao metricDefinitionDao;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        List<Context> contexts = metricDefinitionDao.getAllMetricDefinitionsIds().stream()
            .map(id -> contextOf(id(""), TELEMETRY_SERVICE).child(id, ContextType.METRIC_DEFINITION))
            .sorted().collect(toList());
        return new CollectionDto(CollectionDto.Type.METRIC_DEFINITION, contexts);
    }

    @Transactional(REQUIRED)
    @Override
    public MetricDefinitionDto getResource(Context context) throws ContextResolvingException {
        try {
            MetricDefinition metricDefinition = metricDefinitionDao.find(context.getId());
            return (MetricDefinitionDto) entityToDtoMapper.map(metricDefinition);
        } catch (EntityNotFoundException e) {
            throw new ContextResolvingException(e.getMessage(), context, e);
        }
    }
}
