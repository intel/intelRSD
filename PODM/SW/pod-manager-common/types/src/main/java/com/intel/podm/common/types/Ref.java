/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.common.types;

import java.util.NoSuchElementException;
import java.util.function.Consumer;
import java.util.function.Function;

public abstract class Ref<T> {
    private Ref() {
    }

    public abstract T get();

    public abstract T orElse(T other);

    public boolean isAssigned() {
        return false;
    }

    public abstract <U> Ref<U> map(Function<T, U> function);

    public abstract <U> Ref<U> flatMap(Function<T, Ref<U>> function);

    public void ifAssigned(Consumer<T> consumer) {
    }

    public static <T> Ref<T> of(T value) {
        return new Assigned<>(value);
    }

    public static <V> Ref<V> unassigned() {
        return new Unassigned<>();
    }

    private static final class Unassigned<T> extends Ref<T> {
        @Override
        public T get() {
            throw new NoSuchElementException("Unassigned Ref does not hold any value");
        }

        @Override
        public T orElse(T other) {
            return other;
        }

        @Override
        public <U> Ref<U> map(Function<T, U> function) {
            return Ref.unassigned();
        }

        @Override
        public <U> Ref<U> flatMap(Function<T, Ref<U>> function) {
            return Ref.unassigned();
        }
    }

    private static final class Assigned<T> extends Ref<T> {
        private final T value;

        Assigned(T value) {
            this.value = value;
        }

        @Override
        public T orElse(T other) {
            return value == null ? other : value;
        }

        @Override
        public void ifAssigned(Consumer<T> consumer) {
            consumer.accept(value);
        }

        @Override
        public T get() {
            return value;
        }

        @Override
        public boolean isAssigned() {
            return true;
        }

        @Override
        public <U> Ref<U> map(Function<T, U> function) {
            return Ref.of(function.apply(value));
        }

        @Override
        public <U> Ref<U> flatMap(Function<T, Ref<U>> function) {
            return function.apply(value);
        }
    }
}
