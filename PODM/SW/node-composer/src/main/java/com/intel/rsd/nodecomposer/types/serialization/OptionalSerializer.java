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

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.fasterxml.jackson.databind.ser.std.StdSerializer;
import lombok.NonNull;

import java.io.IOException;
import java.util.Optional;

public class OptionalSerializer extends StdSerializer<Optional> {
    public static final long serialVersionUID = -5019224060053654064L;

    @SuppressWarnings("unchecked")
    public OptionalSerializer(Class clazz) {
        super(clazz);
    }

    @Override
    public void serialize(@NonNull Optional value, JsonGenerator jgen, SerializerProvider provider) throws IOException {
        // to skip a specific property (field name and value), use @JsonInclude(JsonInclude.Inclusion.NON_DEFAULT) on this property
        if (value.isPresent()) {
                jgen.writeObject(value.get());
        } else {
            jgen.writeNull();
        }
    }
}
