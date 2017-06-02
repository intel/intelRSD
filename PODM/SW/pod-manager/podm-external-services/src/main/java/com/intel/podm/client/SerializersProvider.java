/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client;

import com.fasterxml.jackson.databind.module.SimpleModule;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.deserialization.BooleanDeserializer;
import com.intel.podm.common.types.deserialization.EnumeratedTypeDeserializer;
import com.intel.podm.common.types.deserialization.MacAddressDeserializer;
import com.intel.podm.common.types.deserialization.NeighborInfoDeserializer;
import com.intel.podm.common.types.deserialization.RefDeserializer;
import com.intel.podm.common.types.deserialization.StatusDeserializer;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.serialization.EnumeratedTypeSerializer;
import com.intel.podm.common.types.serialization.OptionalSerializer;
import com.intel.podm.common.types.serialization.RefSerializer;

import java.util.Optional;

import static com.intel.podm.common.types.EnumeratedType.SUB_TYPES;

public class SerializersProvider {
    public SimpleModule getSerializersModule() {
        SimpleModule serializersModule = new SimpleModule();

        for (Class subType : SUB_TYPES) {
            serializersModule.addSerializer(subType, new EnumeratedTypeSerializer<>());
            serializersModule.addDeserializer(subType, new EnumeratedTypeDeserializer<>(subType));
        }

        serializersModule.addDeserializer(MacAddress.class, new MacAddressDeserializer());
        serializersModule.addDeserializer(NeighborInfo.class, new NeighborInfoDeserializer());
        serializersModule.addDeserializer(Status.class, new StatusDeserializer());
        serializersModule.addDeserializer(Ref.class, new RefDeserializer());
        serializersModule.addDeserializer(Boolean.class, new BooleanDeserializer());

        serializersModule.addSerializer(Optional.class, new OptionalSerializer(Optional.class));
        serializersModule.addSerializer(Ref.class, new RefSerializer());
        return serializersModule;
    }
}
