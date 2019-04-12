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

package com.intel.rsd.nodecomposer.composition.allocation.mappers;

import com.google.common.base.Function;
import com.google.common.collect.Ordering;
import com.intel.rsd.nodecomposer.business.dto.redfish.ODataIdPossessor;

import java.util.Collection;

import static com.google.common.collect.Ordering.natural;

public final class Sorter {
    private Sorter() {
    }

    @SafeVarargs
    public static <T> Collection<T> sort(Collection<T> toBeSorted, Ordering<T>... orderings) {
        if (orderings == null || orderings.length == 0) {
            throw new IllegalArgumentException("At least one ordering method must be passed to sort.");
        }

        Ordering<T> ordering = orderings[0];
        for (int i = 1; i < orderings.length; i++) {
            ordering = ordering.compound(orderings[i]);
        }

        return ordering.sortedCopy(toBeSorted);
    }

    public static <T, U extends Comparable> Ordering<T> ascendingWhereNullsFirst(Function<T, U> comparingFunction) {
        return natural().nullsFirst().onResultOf(comparingFunction);
    }

    public static <T, U extends Comparable> Ordering<T> ascendingWhereNullsLast(Function<T, U> comparingFunction) {
        return natural().nullsLast().onResultOf(comparingFunction);
    }

    public static <T extends ODataIdPossessor> Ordering<T> urisAscendingWhereNullsLast() {
        return natural().nullsLast().onResultOf(
            addressPossessor -> (addressPossessor == null || addressPossessor.getResourceODataId() == null)
                ? null
                : addressPossessor.getResourceODataId().toUri()
        );
    }
}
