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

package com.intel.rsd.nodecomposer.rest;

import com.fasterxml.jackson.databind.JsonDeserializer;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.intel.rsd.nodecomposer.types.Id;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.deserialization.BooleanDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.MacAddressDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.RefDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.StrictEnumeratedTypeDeserializer;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import com.intel.rsd.nodecomposer.types.serialization.EnumeratedTypeSerializer;
import com.intel.rsd.nodecomposer.types.serialization.IdSerializer;
import com.intel.rsd.nodecomposer.types.serialization.MacAddressSerializer;
import com.intel.rsd.nodecomposer.types.serialization.OffsetDateTimeSerializer;
import com.intel.rsd.nodecomposer.types.serialization.StatusSerializer;

import java.time.OffsetDateTime;
import java.util.LinkedHashMap;
import java.util.Map;

import static com.intel.rsd.nodecomposer.types.serialization.EnumeratedTypeProvider.getSubTypes;

class NorthboundObjectMapperModuleProvider {
    Map<Class<?>, JsonSerializer<?>> getSerializers() {
        Map<Class<?>, JsonSerializer<?>> jsonSerializers = new LinkedHashMap<>();

        for (Class subType : getSubTypes()) {
            jsonSerializers.put(subType, new EnumeratedTypeSerializer<>());
        }

        jsonSerializers.put(OffsetDateTime.class, OffsetDateTimeSerializer.INSTANCE);
        jsonSerializers.put(Id.class, IdSerializer.INSTANCE);
        jsonSerializers.put(MacAddress.class, MacAddressSerializer.INSTANCE);
        jsonSerializers.put(Status.class, StatusSerializer.INSTANCE);

        return jsonSerializers;
    }

    @SuppressWarnings({"unchecked"})
    public Map<Class<?>, JsonDeserializer<?>> getDeserializers() {
        Map<Class<?>, JsonDeserializer<?>> jsonDeserializers = new LinkedHashMap<>();

        for (Class subType : getSubTypes()) {
            jsonDeserializers.put(subType, new StrictEnumeratedTypeDeserializer<>(subType));
        }

        jsonDeserializers.put(MacAddress.class, new MacAddressDeserializer());
        jsonDeserializers.put(Ref.class, new RefDeserializer());
        jsonDeserializers.put(Boolean.class, BooleanDeserializer.INSTANCE);

        return jsonDeserializers;
    }
}
