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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import java.util.ArrayList;
import java.util.Collection;
import java.util.StringJoiner;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

public class FilteringCollection<T> {
    private final Collection<T> collection;
    private final StringJoiner joiner;
    private int counter;

    public FilteringCollection(Collection<T> collection) {
        requiresNonNull(collection, "collection");
        this.collection = new ArrayList<>(collection);
        this.counter = collection.size();
        this.joiner = new StringJoiner(" -> ", "[", "]");
        joiner.add("available: " + collection.size());
    }

    public FilteringCollection<T> filter(Predicate<T> predicate, String predicateName) {
        collection.removeIf(predicate.negate());
        counter = collection.size();
        joiner.add(format("%s: %s", predicateName, counter));
        return this;
    }

    public Collection<T> getCollection() {
        return collection;
    }

    public String getFilterStatistics() {
        return format("Available assets count after applying filters: %s", joiner.toString());
    }
}
