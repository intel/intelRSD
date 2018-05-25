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

package com.intel.podm.business.entities.resolvers;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.transaction.Transactional;
import java.util.Optional;
import java.util.function.Function;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

public abstract class MultiSourceEntityResolver<T extends DiscoverableEntity> implements Function<T, T> {
    private final Class<T> clazz;

    MultiSourceEntityResolver(Class<T> clazz) {
        requiresNonNull(clazz, "clazz");
        this.clazz = clazz;
    }

    public Class<T> getEntityClass() {
        return clazz;
    }

    @Override
    @Transactional(MANDATORY)
    public T apply(T discoverableEntity) {
        if (!discoverableEntity.isComplementary()) {
            return discoverableEntity;
        }

        return findPrimaryEntity(discoverableEntity).orElse(discoverableEntity);
    }

    protected abstract Optional<T> findPrimaryEntity(T complementaryEntity);
    public abstract String createMultiSourceDiscriminator(T entity);
}
