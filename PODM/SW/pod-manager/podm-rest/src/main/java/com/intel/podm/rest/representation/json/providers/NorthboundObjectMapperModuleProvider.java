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

package com.intel.podm.rest.representation.json.providers;

import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.module.SimpleModule;
import com.intel.podm.common.types.EnumeratedType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.deserialization.EnumeratedTypeDeserializer;
import com.intel.podm.common.types.deserialization.MacAddressDeserializer;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.serialization.EnumeratedTypeSerializer;
import com.intel.podm.common.types.serialization.IdSerializer;
import com.intel.podm.common.types.serialization.MacAddressSerializer;
import com.intel.podm.common.types.serialization.NeighborInfoSerializer;
import com.intel.podm.common.types.serialization.OffsetDateTimeSerializer;
import com.intel.podm.common.types.serialization.StatusSerializer;
import com.intel.podm.rest.representation.json.serializers.ContextAwareSerializer;

import javax.enterprise.inject.Instance;
import java.time.OffsetDateTime;

public class NorthboundObjectMapperModuleProvider {

    public SimpleModule getSerializerModule(JsonProvider.SerializerContext context, Instance<ContextAwareSerializer> serializers) {
        SimpleModule module = new SimpleModule();

        for (ContextAwareSerializer serializer : serializers) {
            serializer.setContext(context);
            module.addSerializer((JsonSerializer<?>) serializer);
        }

        for (Class subType : EnumeratedType.SUB_TYPES) {
            module.addSerializer(subType, new EnumeratedTypeSerializer<>());
        }

        module.addSerializer(OffsetDateTime.class, OffsetDateTimeSerializer.INSTANCE);
        module.addSerializer(Id.class, IdSerializer.INSTANCE);
        module.addSerializer(MacAddress.class, MacAddressSerializer.INSTANCE);
        module.addSerializer(NeighborInfo.class, NeighborInfoSerializer.INSTANCE);
        module.addSerializer(Status.class, StatusSerializer.INSTANCE);

        return module;
    }

    public SimpleModule getDeserializerModule() {
        SimpleModule module = new SimpleModule();

        for (Class subType : EnumeratedType.SUB_TYPES) {
            module.addDeserializer(subType, new EnumeratedTypeDeserializer<>(subType));
        }

        module.addDeserializer(MacAddress.class, new MacAddressDeserializer());

        return module;
    }
}
