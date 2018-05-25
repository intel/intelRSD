/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.config.base;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.module.SimpleModule;
import com.intel.podm.common.types.deserialization.EnumeratedTypeDeserializer;
import com.intel.podm.common.types.deserialization.MacAddressDeserializer;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Produces;
import javax.inject.Named;

import static com.fasterxml.jackson.databind.SerializationFeature.INDENT_OUTPUT;
import static com.intel.podm.common.types.EnumeratedType.SUB_TYPES;

@Dependent
public class ObjectMapperFactory {
    @Produces @Named("ConfigMapper")
    public ObjectMapper create() {
        SimpleModule deserializersModule = new SimpleModule();

        for (Class subType : SUB_TYPES) {
            deserializersModule.addDeserializer(subType, new EnumeratedTypeDeserializer<>(subType));
        }

        deserializersModule.addDeserializer(MacAddress.class, new MacAddressDeserializer());

        return new ObjectMapper()
                .registerModule(deserializersModule)
                .enable(INDENT_OUTPUT);
    }
}
