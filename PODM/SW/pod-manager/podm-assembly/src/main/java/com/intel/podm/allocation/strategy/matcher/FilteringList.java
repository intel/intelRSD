/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.business.entities.redfish.ComputerSystem;

import java.util.List;
import java.util.StringJoiner;
import java.util.function.Predicate;

import static java.lang.String.format;

public class FilteringList {
    private final List<ComputerSystem> computerSystems;
    private int counter;
    private final StringJoiner joiner;

    FilteringList(List<ComputerSystem> computerSystems) {
        if (computerSystems == null) {
            throw new IllegalArgumentException("ComputerSystemsList should not be null");
        }
        this.computerSystems = computerSystems;
        this.counter = computerSystems.size();
        this.joiner = new StringJoiner(" -> ", "[", "]");
        joiner.add("available: " + computerSystems.size());
    }

    public FilteringList filter(Predicate<ComputerSystem> predicate, String predicateName) {
        computerSystems.removeIf(predicate.negate());
        counter = computerSystems.size();
        joiner.add(format("%s: %s", predicateName, counter));
        return this;
    }

    public List<ComputerSystem> getComputerSystems() {
        return computerSystems;
    }

    public String getFilterStatistics() {
        return format("Available assets count after applying filters: %s", joiner.toString());
    }
}
