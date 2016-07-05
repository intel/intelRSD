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

import com.intel.podm.business.entities.base.converters.EnumConverter;
import com.intel.podm.business.entities.base.converters.EnumeratedTypeListConverter;
import com.intel.podm.business.entities.base.converters.ListConverter;
import com.intel.podm.business.entities.base.converters.MacAddressConverter;
import com.intel.podm.business.entities.base.converters.NeighborInfoConverter;
import com.intel.podm.business.entities.base.converters.StatusConverter;
import com.intel.podm.business.entities.base.converters.UriConverter;
import com.intel.podm.business.entities.base.converters.UuidConverter;
import com.intel.podm.common.types.EnumeratedType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.helpers.EnumeratedTypeListHolder;
import com.intel.podm.common.types.net.MacAddress;

import java.math.BigDecimal;
import java.net.URI;
import java.util.List;
import java.util.UUID;

public final class DomainObjectProperties {
    private DomainObjectProperties() {
    }

    public static DomainObjectProperty<String> stringProperty(String name) {
        return DomainObjectProperty.property(name, String.class);
    }

    public static DomainObjectProperty<Integer> integerProperty(String name) {
        return DomainObjectProperty.property(name, Integer.class);
    }

    public static DomainObjectProperty<Boolean> booleanProperty(String name) {
        return DomainObjectProperty.property(name, Boolean.class);
    }

    public static DomainObjectProperty<BigDecimal> decimalProperty(String name) {
        return DomainObjectProperty.property(name, BigDecimal.class);
    }

    public static <T> DomainObjectProperty<List<T>> listProperty(String name) {
        return DomainObjectProperty.property(name, List.class, new ListConverter());
    }

    public static <T extends Enum> DomainObjectProperty<T> enumProperty(String name, Class<T> aClass) {
        return DomainObjectProperty.property(name, String.class, new EnumConverter<>(aClass));
    }

    public static <T extends EnumeratedType> DomainObjectProperty<EnumeratedTypeListHolder<T>> enumeratedTypeListProperty(String name, Class<T> enumType) {
        return DomainObjectProperty.property(name, String.class, new EnumeratedTypeListConverter<>(enumType));
    }

    public static DomainObjectProperty<UUID> uuidProperty(String name) {
        return DomainObjectProperty.property(name, String.class, new UuidConverter());
    }

    public static DomainObjectProperty<URI> uriProperty(String name) {
        return DomainObjectProperty.property(name, String.class, new UriConverter());
    }

    public static DomainObjectProperty<MacAddress> macAddressProperty(String macAddressString) {
        return DomainObjectProperty.property(macAddressString, String.class, new MacAddressConverter());
    }

    public static DomainObjectProperty<Status> statusProperty(String name) {
        return DomainObjectProperty.property(name, String.class, new StatusConverter());
    }

    public static DomainObjectProperty<NeighborInfo> neighborInfoProperty(String neighborInfoString) {
        return DomainObjectProperty.property(neighborInfoString, String.class, new NeighborInfoConverter());
    }
}
