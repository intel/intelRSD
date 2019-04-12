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
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;

import java.io.IOException;
import java.time.OffsetDateTime;
import java.time.format.DateTimeFormatter;

/**
 * Serializes OffsetDateTime class to JSON String class with date format "yyyy-MM-dd'T'HH:mm:ssxxx"
 * Example: "2015-04-09T11:00:00+00:00"
 */
public class OffsetDateTimeSerializer extends JsonSerializer<OffsetDateTime> {
    public static final OffsetDateTimeSerializer INSTANCE = new OffsetDateTimeSerializer();

    private static final String DATE_FORMAT = "yyyy-MM-dd'T'HH:mm:ssxxx";

    @Override
    public void serialize(OffsetDateTime time, JsonGenerator jsonGenerator, SerializerProvider serializerProvider)
            throws IOException {
        jsonGenerator.writeString(time.format(DateTimeFormatter.ofPattern(DATE_FORMAT)));
    }
}
