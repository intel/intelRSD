/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.eventservice.data;

import com.intel.rsd.dto.ODataId;
import lombok.NonNull;

import javax.persistence.AttributeConverter;
import javax.persistence.Converter;

@Converter(autoApply = true)
public class ODataIdConverter implements AttributeConverter<ODataId, String> {
    @Override
    public String convertToDatabaseColumn(@NonNull ODataId entityODataId) {
        return entityODataId.getValue().toString();
    }

    @Override
    public ODataId convertToEntityAttribute(@NonNull String databaseODataId) {
        return ODataId.odataId(databaseODataId);
    }
}
