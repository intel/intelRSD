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

package com.intel.podm.business.entities.base;

import com.intel.podm.business.entities.base.converters.NopConverter;
import com.intel.podm.business.entities.base.converters.PropertyTypeConverter;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.google.common.base.Strings.isNullOrEmpty;

public final class DomainObjectProperty<T> {
    private final String name;
    private final Class<?> type;

    private final PropertyTypeConverter converter;

    private DomainObjectProperty(String name, Class<?> type, PropertyTypeConverter converter) {
        if (isNullOrEmpty(name)) {
            throw new IllegalArgumentException("name should not be null or empty");
        }

        this.name = name;
        this.type = checkNotNull(type);
        this.converter = checkNotNull(converter);
    }

    public Object convertFromMappedType(Object o) {
        return converter.convertFromMappedType(o);
    }

    public Object convertToMappedType(Object o) {
        return converter.convertToMappedType(o);
    }

    public static <X, Y> DomainObjectProperty<Y> property(String name, Class<X> type, PropertyTypeConverter<X, Y> converter) {
        return new DomainObjectProperty<>(name, type, converter);
    }

    public static <X> DomainObjectProperty<X> property(String name, Class<X> type) {
        return new DomainObjectProperty<>(name, type, new NopConverter<X>());
    }

    public String getName() {
        return name;
    }

    public Class<?> getType() {
        return type;
    }
}
