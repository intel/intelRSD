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

package com.intel.rsd.nodecomposer.utils;

import java.util.function.Supplier;

public final class Contracts {
    private Contracts() {
    }

    public static void checkState(boolean expression, String message) {
        if (!expression) {
            throw new IllegalStateException(message);
        }
    }

    public static void requires(boolean expression, String message) {
        checkArgument(expression, () -> new IllegalArgumentException(message));
    }

    public static <T extends Throwable> void checkArgument(boolean expression, Supplier<T> supplier) throws T {
        if (!expression) {
            throw supplier.get();
        }
    }

    public static <T> T requiresNonNull(T arg, String argName) {
        return requiresNonNull(arg, argName, "cannot be null");
    }

    public static <T> T requiresNonNull(T arg, String argName, String message) {
        if (arg == null) {
            throw new IllegalArgumentException(String.valueOf(argName) + ": " + message);
        }

        return arg;
    }

    public static <T, E extends Throwable> T requiresNonNull(T arg, Supplier<E> supplier) throws E {
        if (arg == null) {
            throw supplier.get();
        }

        return arg;
    }
}
