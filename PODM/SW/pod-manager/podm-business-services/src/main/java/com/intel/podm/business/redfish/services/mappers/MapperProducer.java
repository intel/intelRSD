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

import com.intel.podm.business.dto.RedfishDto;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.Collection;
import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

@ApplicationScoped
public class MapperProducer {
    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Inject
    private Instance<DtoMapper<? extends Entity, ? extends RedfishDto>> mapperPool;

    private Collection<DtoMapper<? extends Entity, ? extends RedfishDto>> mapperCache;

    public Optional<DtoMapper<? extends Entity, ? extends RedfishDto>> tryFindDtoMapperForEntity(Class<? extends Entity> entityClass) {
        Optional<DtoMapper<? extends Entity, ? extends RedfishDto>> possibleDtoMapper = tryGetDtoMapperFromPool(entityClass);
        if (possibleDtoMapper.isPresent()) {
            DtoMapper<? extends Entity, ? extends RedfishDto> dtoMapper = possibleDtoMapper.get();
            dtoMapper.setUnknownOemTranslator(unknownOemTranslator);
            return of(dtoMapper);
        } else {
            return empty();
        }
    }

    private Optional<DtoMapper<? extends Entity, ? extends RedfishDto>> tryGetDtoMapperFromPool(Class<? extends Entity> entityClass) {
        return stream(getMapperPool().spliterator(), false)
            .filter(mapper -> mapper.canMap(entityClass))
            .findFirst();
    }

    private Iterable<DtoMapper<? extends Entity, ? extends RedfishDto>> getMapperPool() {
        if (mapperCache == null) {
            mapperCache = stream(mapperPool.spliterator(), false).collect(toList());
        }

        return mapperCache;
    }
}
