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

package com.intel.rsd.nodecomposer.persistence.converters;

import javax.persistence.AttributeConverter;
import javax.persistence.Converter;
import java.util.Objects;
import java.util.UUID;

import static java.util.UUID.fromString;

@Converter(autoApply = true)
public class UuidConverter implements AttributeConverter<UUID, String> {
    @Override
    public String convertToDatabaseColumn(UUID entityUuid) {
        return Objects.toString(entityUuid, null);
    }

    @Override
    public UUID convertToEntityAttribute(String databaseUuid) {
        return databaseUuid == null
                ? null
                : fromString(databaseUuid);
    }
}
