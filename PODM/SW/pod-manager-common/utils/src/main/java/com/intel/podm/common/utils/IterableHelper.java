/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.common.utils;

import java.util.Iterator;

public final class IterableHelper {

    private IterableHelper() {
    }

    /**
     * Method returns the single element from an iterable.
     *
     * @param iterable  any class implementing Iterable<T> interface
     * @param <T>       class of iterable element
     * @return          single object of class T
     *
     * @throws  IllegalArgumentException
     *          If null is passed as an argument.
     *
     * @throws  IllegalStateException
     *          If iterable is empty or contains more than one element.
     */
    public static <T> T single(Iterable<T> iterable) {
        if (iterable == null) {
            throw new IllegalArgumentException("Null is not a valid argument for this method");
        }
        Iterator<T> iterator = iterable.iterator();
        if (!iterator.hasNext()) {
            throw new IllegalStateException("Iterator is empty. Only iterator with 1 element is accepted!");
        } else {
            T element = iterator.next();
            if (iterator.hasNext()) {
                throw new IllegalStateException("More than one element in iterator. Only iterator with 1 element is accepted!");
            }
            return element;
        }
    }

    /**
     * Method returns the single element from an iterable or null if empty iterable is passed.
     *
     * @param iterable  any class implementing Iterable<T> interface
     * @param <T>       class of iterable element
     * @return          single object of class T or null
     *
     * @throws  IllegalArgumentException
     *          If null is passed as an argument.
     *
     * @throws  IllegalStateException
     *          If iterable contains more than one element.
     */
    public static <T> T singleOrNull(Iterable<T> iterable) {
        if (iterable == null) {
            throw new IllegalArgumentException("Null is not a valid argument for this method");
        }
        Iterator<T> iterator = iterable.iterator();
        if (!iterable.iterator().hasNext()) {
            return null;
        } else {
            T element = iterator.next();
            if (iterator.hasNext()) {
                throw new IllegalStateException("Only iterators with 1 or 0 elements are accepted!");
            }
            return element;
        }
    }
}
