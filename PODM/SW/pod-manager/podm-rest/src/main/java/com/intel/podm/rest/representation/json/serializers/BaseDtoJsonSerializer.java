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

package com.intel.podm.rest.representation.json.serializers;

import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.intel.podm.business.dto.redfish.BaseDto;
import com.intel.podm.redfish.json.templates.BaseJson;

import java.io.IOException;

import static com.intel.podm.rest.representation.json.serializers.OemSerializeHelper.oemDtosToUnknownOems;

public abstract class BaseDtoJsonSerializer<T extends BaseDto> extends DtoJsonSerializer<T> implements ContextAwareSerializer {
    protected BaseDtoJsonSerializer(Class<T> dtoType) {
        super(dtoType);
    }

    @Override
    public final void serialize(T value, JsonGenerator gen, SerializerProvider serializers) throws IOException {
        BaseJson baseJson = translate(value);
        baseJson.unknownOems = oemDtosToUnknownOems(value.getUnknownOems());
        gen.writeObject(baseJson);
    }
}
