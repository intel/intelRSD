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

package com.intel.podm.business.entities;

import com.intel.podm.common.types.Pair;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.BiPredicate;

import static java.util.Arrays.asList;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;
import static java.util.stream.IntStream.range;

public final class Diff {

    private static final BiPredicate<Object, Object> NON_NULL_AND_NON_EQUAL
        = (previous, current) -> Objects.nonNull(previous) && Objects.nonNull(current) && !previous.equals(current);

    private final Object[] previousState;
    private final Object[] currentState;
    private final String[] propertyNames;

    public Diff(final Object[] previousState, final Object[] currentState, final String[] propertyNames) {
        this.previousState = previousState;
        this.currentState = currentState;
        this.propertyNames = propertyNames;
    }

    public <A> Optional<Pair<A, A>> getPairFor(String propertyName) {
        return getIndexForProperty(propertyName).map(index ->
            Pair.pairOf((A) previousState[index], (A) currentState[index])
        );
    }

    public Boolean wasUpdatedFromOneValueToAnother(String propertyName) {
        return getIndexForProperty(propertyName)
            .map(index -> NON_NULL_AND_NON_EQUAL.test(previousState[index], currentState[index]))
            .orElse(false);
    }

    private Optional<Integer> getIndexForProperty(String propertyName) {
        return ofNullable(
            zipWithIndex(asList(propertyNames)).stream()
                .collect(toMap(Pair::first, Pair::second))
                .get(propertyName)
        );
    }

    private static <C> List<Pair<C, Integer>> zipWithIndex(List<C> list) {
        return zip(
            list,
            range(0, list.size()).boxed().collect(toList())
        );
    }

    private static <A, B> List<Pair<A, B>> zip(List<A> firstList, List<B> secondList) {
        Iterator<A> firstIterator = firstList.iterator();
        Iterator<B> secondIterator = secondList.iterator();

        ArrayList<Pair<A, B>> result = new ArrayList<>();

        while (firstIterator.hasNext() && secondIterator.hasNext()) {
            result.add(Pair.pairOf(firstIterator.next(), secondIterator.next()));
        }

        return result;
    }
}
