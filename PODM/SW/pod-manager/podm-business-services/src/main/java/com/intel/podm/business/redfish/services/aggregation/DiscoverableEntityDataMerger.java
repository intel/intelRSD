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

package com.intel.podm.business.redfish.services.aggregation;

import com.intel.podm.business.dto.RedfishDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.redfish.services.mappers.DtoMapper;
import com.intel.podm.business.redfish.services.mappers.MapperProducer;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.InBandServiceConfig;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.intel.podm.mappers.Conditions.aggregateCondition;
import static java.lang.String.format;
import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;
import static javax.transaction.Transactional.TxType.MANDATORY;

public abstract class DiscoverableEntityDataMerger<T extends DiscoverableEntity, S extends RedfishDto> {
    @Inject
    @Config
    private Holder<InBandServiceConfig> inBandServiceConfig;

    @Inject
    private MapperProducer mapperProducer;

    @Transactional(MANDATORY)
    public S toDto(T discoverableEntity) {
        DtoMapper<T, S> mapper = getDtoMapperForDiscoverableEntity(discoverableEntity);
        S dto = mapper.createDto();
        mapper.map(discoverableEntity, dto);
        List<UnknownOemDto> mergedUnknownOems = dto.getUnknownOems();

        if (inBandServiceConfig.get().isInBandServiceSupportEnabled()) {
            mapper.setMappingConditions(aggregateCondition(true, false));
            List<T> multiSourceRepresentations = getMultiSourceRepresentations(discoverableEntity);
            for (T multiSourceRepresentation : multiSourceRepresentations) {
                mapper.map(multiSourceRepresentation, dto);
                mergedUnknownOems = mergeOems(mergedUnknownOems, dto.getUnknownOems());
            }
        }
        dto.setId(discoverableEntity.getId().toString());
        dto.setUnknownOems(mergedUnknownOems);
        return dto;
    }

    @SuppressWarnings({"unchecked"})
    private DtoMapper<T, S> getDtoMapperForDiscoverableEntity(T entity) {
        return (DtoMapper<T, S>) mapperProducer.tryFindDtoMapperForEntity(entity.getClass())
            .orElseThrow(() -> new RuntimeException(format("Could not find proper mapper for: %s", entity.getClass())));
    }

    /**
     * Implementation should provide additional representations of base entity,
     * entities provided by this list will be processed using this List's order.
     *
     * @param entity base entity for merge
     * @return list of other representations of base entity
     */
    protected abstract List<T> getMultiSourceRepresentations(T entity);

    private List<UnknownOemDto> mergeOems(List<UnknownOemDto> actualOems, List<UnknownOemDto> newOems) {
        Map<String, UnknownOemDto> mergedOems = actualOems.stream().collect(toMap(UnknownOemDto::getOemPath, identity()));
        for (UnknownOemDto newOem : newOems) {
            mergedOems.put(newOem.getOemPath(), newOem);
        }
        return new ArrayList<>(mergedOems.values());
    }
}
