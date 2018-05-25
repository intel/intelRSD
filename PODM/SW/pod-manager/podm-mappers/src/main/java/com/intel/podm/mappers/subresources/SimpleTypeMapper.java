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

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.function.Consumer;

import static com.intel.podm.common.utils.Collections.emptyIfNull;
import static com.intel.podm.common.utils.Collections.equalAndDontContainNulls;

@Dependent
public class SimpleTypeMapper {
    public <T> void map(Collection<T> sourceCollection, Collection<T> targetCollection, Consumer<T> targetConsumer) {
        Collection<T> safeSourceCollection = emptyIfNull(sourceCollection);

        if (equals(safeSourceCollection, targetCollection)) {
            return;
        }

        targetCollection.clear();
        safeSourceCollection.forEach(targetConsumer);
    }

    public <T> boolean equals(Collection<T> sourceCollection, Collection<T> targetCollection) {
        return equalAndDontContainNulls(sourceCollection, targetCollection);
    }
}
