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

package com.intel.rsd.nodecomposer.types;

import javax.annotation.Nonnull;
import java.io.Serializable;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

public final class Id implements Comparable<Id>, Serializable {
    private static final long serialVersionUID = -4051352713257896901L;

    private final String value;

    private Id(long value) {
        this.value = Long.toString(value);
    }

    private Id(String value) {
        requiresNonNull(value, "value");
        this.value = value;
    }

    public static Id id(long value) {
        return new Id(value);
    }

    public static Id id(String value) {
        return new Id(value);
    }

    /* used by JAX-RS */
    public static Id fromString(String value) {
        return new Id(value);
    }

    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return value;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Id id = (Id) o;
        return value.equals(id.value);
    }

    @Override
    public int hashCode() {
        return value.hashCode();
    }

    @Override
    public int compareTo(@Nonnull Id anotherId) {
        return value.compareTo(anotherId.getValue());
    }
}
