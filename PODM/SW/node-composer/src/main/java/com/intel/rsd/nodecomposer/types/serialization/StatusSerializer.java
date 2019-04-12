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

package com.intel.rsd.nodecomposer.types.serialization;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.intel.rsd.nodecomposer.types.Status;

import java.io.IOException;

public class StatusSerializer extends JsonSerializer<Status> {
    public static final JsonSerializer<Status> INSTANCE = new StatusSerializer();

    @Override
    public void serialize(Status value, JsonGenerator jgen, SerializerProvider provider) throws IOException {
        jgen.writeObject(new Object() {
            @JsonProperty("State") Object state = value.getState();
            @JsonProperty("Health") Object health = value.getHealth();
            @JsonProperty("HealthRollup") Object healthRollup = value.getHealthRollup();
        });
    }
}
