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

import com.intel.rsd.nodecomposer.types.Status;

import javax.persistence.AttributeConverter;
import javax.persistence.Converter;
import java.util.Objects;

import static com.intel.rsd.nodecomposer.types.Status.statusFromString;

@Converter(autoApply = true)
public class StatusConverter implements AttributeConverter<Status, String> {
    @Override
    public String convertToDatabaseColumn(Status entityStatus) {
        return Objects.toString(entityStatus, null);
    }

    @Override
    public Status convertToEntityAttribute(String databaseStatus) {
        return statusFromString(databaseStatus);
    }
}

