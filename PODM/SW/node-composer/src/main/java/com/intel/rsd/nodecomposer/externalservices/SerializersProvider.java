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

package com.intel.rsd.nodecomposer.externalservices;

import com.fasterxml.jackson.databind.module.SimpleModule;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.deserialization.BooleanDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.EnumeratedTypeDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.MacAddressDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.RefDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.StatusDeserializer;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import com.intel.rsd.nodecomposer.types.serialization.EnumeratedTypeSerializer;
import com.intel.rsd.nodecomposer.types.serialization.OptionalSerializer;
import com.intel.rsd.nodecomposer.types.serialization.RefSerializer;

import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.serialization.EnumeratedTypeProvider.getSubTypes;

public class SerializersProvider {
    @SuppressWarnings({"unchecked"})
    public SimpleModule getSerializersModule() {
        SimpleModule serializersModule = new SimpleModule();

        for (Class subType : getSubTypes()) {
            serializersModule.addSerializer(subType, new EnumeratedTypeSerializer<>());
            serializersModule.addDeserializer(subType, new EnumeratedTypeDeserializer<>(subType));
        }

        serializersModule.addDeserializer(MacAddress.class, new MacAddressDeserializer());
        serializersModule.addDeserializer(Status.class, new StatusDeserializer());
        serializersModule.addDeserializer(Ref.class, new RefDeserializer());
        serializersModule.addDeserializer(Boolean.class, new BooleanDeserializer());

        serializersModule.addSerializer(Optional.class, new OptionalSerializer(Optional.class));
        serializersModule.addSerializer(Ref.class, new RefSerializer());
        return serializersModule;
    }
}
