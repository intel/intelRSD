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

import static java.lang.String.format;
import static java.util.stream.Collectors.collectingAndThen;
import static java.util.stream.Collectors.toList;

public final class Collector {
    private Collector() {
    }

    public static <T> java.util.stream.Collector<T, ?, T> toSingle() {
        return collectingAndThen(
            toList(),
            list -> {
                if (list.size() != 1) {
                    throw new IllegalStateException(format("There must be exactly one element in provided collection. Collection size %s", list.size()));
                }
                return list.get(0);
            }
        );
    }

    public static <T> java.util.stream.Collector<T, ?, T> toSingleOrNull() {
        return collectingAndThen(
            toList(),
            list -> {
                if (list.size() > 1) {
                    throw new IllegalStateException("There must be zero or one element in provided collection");
                }
                return list.stream().findFirst().orElse(null);
            }
        );
    }
}
