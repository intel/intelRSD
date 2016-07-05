/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.common.utils;

import java.util.Collection;
import java.util.Objects;

import static com.google.common.base.Preconditions.checkNotNull;
import static java.util.stream.Collectors.toList;

public final class Collections {

    private Collections() {
    }

    public static <T, U extends T> Collection<U> filterByType(Collection<T> collection, Class<U> selectedClass) {
        checkNotNull(collection, "collection argument cannot be null");
        checkNotNull(selectedClass, "selectedClass argument cannot be null");

        return collection.stream()
                .filter(Objects::nonNull)
                .filter(object -> selectedClass.isAssignableFrom(object.getClass()))
                .map(selectedClass::cast)
                .collect(toList());
    }
}
