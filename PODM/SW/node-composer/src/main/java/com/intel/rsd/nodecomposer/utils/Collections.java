/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils;

import java.util.Collection;
import java.util.Iterator;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

public final class Collections {
    private Collections() {
    }

    public static <T, U> Collection<U> filterByType(Collection<T> collection, Class<U> selectedClass) {
        requiresNonNull(collection, "collection");
        requiresNonNull(selectedClass, "selectedClass");

        return collection.stream()
            .filter(Objects::nonNull)
            .filter(selectedClass::isInstance)
            .map(selectedClass::cast)
            .collect(toList());
    }

    public static <T> Optional<T> firstByPredicate(Collection<T> collection, Predicate<? super T> withPredicate) {
        requiresNonNull(collection, "collection");
        requiresNonNull(withPredicate, "predicate");

        return collection.stream()
            .filter(Objects::nonNull)
            .filter(withPredicate)
            .findFirst();
    }

    public static boolean nonEmptyAndSameSize(Collection collection1, Collection collection2) {
        return !nullOrEmpty(collection1) && !nullOrEmpty(collection2) && collection1.size() == collection2.size();
    }

    public static boolean nullOrEmpty(Collection collection) {
        return collection == null || collection.isEmpty();
    }

    public static <T> Collection<T> emptyIfNull(Collection<T> iterable) {
        return iterable == null ? emptyList() : iterable;
    }

    public static boolean equalAndDontContainNulls(Collection sourceCollection, Collection targetCollection) {
        if (!nonEmptyAndSameSize(sourceCollection, targetCollection)) {
            return false;
        }

        Iterator sourceIterator = sourceCollection.iterator();
        Iterator targetIterator = targetCollection.iterator();

        while (sourceIterator.hasNext()) {
            Object source = sourceIterator.next();
            Object target = targetIterator.next();

            if (source == null || target == null || !Objects.equals(source, target)) {
                return false;
            }
        }

        return true;
    }
}
