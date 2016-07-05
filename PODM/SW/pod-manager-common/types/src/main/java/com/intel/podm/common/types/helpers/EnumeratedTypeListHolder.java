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

package com.intel.podm.common.types.helpers;

import com.intel.podm.common.types.EnumeratedType;
import com.intel.podm.common.utils.StringRepresentation;

import java.util.LinkedList;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.podm.common.utils.StringRepresentation.fromList;
import static java.util.Collections.emptyList;
import static java.util.Collections.unmodifiableList;
import static java.util.stream.Collectors.toCollection;

public final class EnumeratedTypeListHolder<T extends EnumeratedType> {
    private final Class<T> enumType;
    private final List<T> elements;

    private EnumeratedTypeListHolder(Class<T> enumType, List<T> elements) {
        this.enumType = enumType;
        if (elements == null) {
            this.elements = unmodifiableList(new LinkedList<>());
        } else {
            this.elements = unmodifiableList(new LinkedList<>(elements));
        }
    }

    private EnumeratedTypeListHolder(Class<T> enumType, String text) {
        this.enumType = enumType;
        if (isNullOrEmpty(text)) {
            this.elements = unmodifiableList(new LinkedList<>());
        } else {
            List<String> stringElementList = StringRepresentation.toList(text, true);

            List<T> elements = stringElementList
                    .stream()
                    .map(stringElement -> EnumeratedType.stringToEnum(this.enumType, stringElement))
                    .collect(Collectors.toList());

            this.elements = unmodifiableList(elements);
        }
    }

    public static <E extends EnumeratedType> EnumeratedTypeListHolder<E> enumeratedTypeListHolder(Class<E> enumType, List<E> elements) {
        return new EnumeratedTypeListHolder<>(enumType, elements);
    }

    public static <E extends EnumeratedType> EnumeratedTypeListHolder<E> enumeratedTypeListHolder(Class<E> enumType, String text) {
        return new EnumeratedTypeListHolder<>(enumType, text);
    }

    public static <E extends EnumeratedType> List<E> toList(Class<E> enumType, EnumeratedTypeListHolder<E> enumeratedTypeListHolder) {
        return enumeratedTypeListHolder != null ? enumeratedTypeListHolder.toList() : empty(enumType).toList();
    }

    public static <E extends EnumeratedType> EnumeratedTypeListHolder<E> empty(Class<E> enumType) {
        return new EnumeratedTypeListHolder<>(enumType, emptyList());
    }

    public List<T> toList() {
        return elements;
    }

    public List<String> toStringList() {
        return elements
                .stream()
                .map(T::getValue)
                .collect(toCollection(LinkedList::new));
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof EnumeratedTypeListHolder)) {
            return false;
        }

        EnumeratedTypeListHolder otherEnumeratedTypeListHolder = (EnumeratedTypeListHolder) o;

        return Objects.equals(elements, otherEnumeratedTypeListHolder.elements);
    }

    @Override
    public int hashCode() {
        return elements != null ? elements.hashCode() : 0;
    }

    @Override
    public String toString() {
        if (elements == null || elements.isEmpty()) {
            return "";
        }

        return fromList(toStringList());
    }
}
