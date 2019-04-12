/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.java.lang;

import lombok.EqualsAndHashCode;

@EqualsAndHashCode
public final class Pair<A, B> {
    private final A first;
    private final B second;

    private Pair(A a, B b) {
        this.first = a;
        this.second = b;
    }

    public static <A, B> Pair<A, B> pairOf(A first, B second) {
        return new Pair<>(first, second);
    }

    public A first() {
        return first;
    }

    public B second() {
        return second;
    }
}
