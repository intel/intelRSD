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

package com.intel.rsd.nodecomposer.types;

import java.io.Serializable;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.function.Consumer;
import java.util.function.Function;
import java.util.function.Supplier;

public abstract class Ref<T> implements Serializable {
    private Ref() {
    }

    public static <T> Ref<T> of(T value) {
        return new Assigned<>(value);
    }

    public static <V> Ref<V> unassigned() {
        return new Unassigned<>();
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

    public <X extends Throwable> T orElseThrow(Supplier<? extends X> exceptionSupplier) throws X {
        if (isAssigned()) {
            return get();
        } else {
            throw exceptionSupplier.get();
        }
    }

    private static final class Unassigned<T> extends Ref<T> implements Serializable {
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

        @Override
        public int hashCode() {
            return Objects.hash(isAssigned());
        }

        @Override
        public boolean equals(Object o) {
            return this == o || o instanceof Unassigned;
        }
    }

    private static final class Assigned<T> extends Ref<T> implements Serializable {
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

        @Override
        public int hashCode() {
            if (this.get() != null) {
                return Objects.hash(isAssigned(), this.get());
            }

            return Objects.hash(isAssigned());
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (!(o instanceof Assigned)) {
                return false;
            }
            Ref that = (Ref) o;

            return Objects.equals(this.get(), that.get());
        }
    }
}
