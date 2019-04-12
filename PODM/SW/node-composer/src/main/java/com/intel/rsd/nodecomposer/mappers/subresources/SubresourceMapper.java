/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.subresources;

import com.intel.rsd.nodecomposer.mappers.subresources.strategies.CleanAndCreateStrategy;

import java.util.Collection;
import java.util.Iterator;
import java.util.function.Consumer;

import static com.intel.rsd.nodecomposer.utils.Collections.emptyIfNull;
import static com.intel.rsd.nodecomposer.utils.Collections.nonEmptyAndSameSize;

public abstract class SubresourceMapper<SOURCE, TARGET> {
    private final Class<TARGET> targetClass;
    private final CleanAndCreateStrategy<TARGET> cleanAndCreateStrategy;

    public SubresourceMapper(Class<TARGET> targetClass, CleanAndCreateStrategy<TARGET> cleanAndCreateStrategy) {
        this.targetClass = targetClass;
        this.cleanAndCreateStrategy = cleanAndCreateStrategy;
    }

    public abstract void map(SOURCE source, TARGET target);
    protected abstract boolean equals(SOURCE source, TARGET target);

    public void map(Collection<SOURCE> sourceCollection, Collection<TARGET> targetCollection, Consumer<TARGET> targetConsumer) {
        Collection<SOURCE> safeSourceCollection = emptyIfNull(sourceCollection);

        if (equalAndDontContainNulls(safeSourceCollection, targetCollection)) {
            return;
        }

        cleanAndCreateStrategy.cleanExistingCollection(targetCollection);

        for (SOURCE source : safeSourceCollection) {
            TARGET target = cleanAndCreateStrategy.provideNew(targetClass);
            map(source, target);
            targetConsumer.accept(target);
        }
    }

    public boolean equalAndDontContainNulls(Collection sourceCollection, Collection targetCollection) {
        if (!nonEmptyAndSameSize(sourceCollection, targetCollection)) {
            return false;
        }

        Iterator<SOURCE> sourceIterator = sourceCollection.iterator();
        Iterator<TARGET> targetIterator = targetCollection.iterator();

        while (sourceIterator.hasNext()) {
            SOURCE source = sourceIterator.next();
            TARGET target = targetIterator.next();

            if (source == null || target == null || !equals(source, target)) {
                return false;
            }
        }

        return true;
    }
}
