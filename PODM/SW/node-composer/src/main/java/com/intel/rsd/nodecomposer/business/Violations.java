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

package com.intel.rsd.nodecomposer.business;

import org.apache.commons.lang3.builder.ToStringBuilder;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Spliterator;
import java.util.function.Consumer;

import static java.lang.String.format;
import static java.util.Collections.unmodifiableList;

public final class Violations implements Iterable<String>, Serializable {
    private List<String> violations = new ArrayList<>();

    public static Violations createWithViolations(String... violations) {
        Violations combinedViolations = new Violations();

        for (String violation : violations) {
            combinedViolations.addViolation(violation);
        }

        return combinedViolations;
    }

    public Violations addViolation(String violation) {
        violations.add(violation);
        return this;
    }

    public void addMissingPropertyViolation(String propertyName) {
        violations.add(format("Mandatory property %s is missing", propertyName));
    }

    public void addValueNotAllowedViolation(String propertyName, Collection<?> allowedValues) {
        violations.add(format("Value of %s property is not allowed. Allowed values are: %s", propertyName, allowedValues));
    }

    public static Violations ofValueNotAllowedViolation(String propertyName, Collection<?> allowedValues) {
        Violations violation = new Violations();
        violation.addValueNotAllowedViolation(propertyName, allowedValues);
        return violation;
    }

    @Override
    public Iterator<String> iterator() {
        return violations.iterator();
    }

    @Override
    public void forEach(Consumer<? super String> consumer) {
        violations.forEach(consumer);
    }

    @Override
    public Spliterator<String> spliterator() {
        return violations.spliterator();
    }

    public boolean hasViolations() {
        return !violations.isEmpty();
    }

    public List<String> asStringList() {
        return unmodifiableList(this.violations);
    }

    public Violations addAll(Violations violations) {
        if (violations != null) {
            this.violations.addAll(violations.asStringList());
        }
        return this;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
            .append("violations", violations)
            .toString();
    }
}
