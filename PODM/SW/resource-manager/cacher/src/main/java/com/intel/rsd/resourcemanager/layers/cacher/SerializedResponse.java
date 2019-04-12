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

package com.intel.rsd.resourcemanager.layers.cacher;

import com.intel.rsd.resourcemanager.layers.Response;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

import java.io.Serializable;
import java.time.LocalDateTime;

import static java.time.LocalDateTime.now;

@Data
@NoArgsConstructor
public class SerializedResponse implements Serializable {
    private static final ResponseToStringConverter RESPONSE_TO_STRING_CONVERTER = new ResponseToStringConverter();
    private String response;
    private LocalDateTime lastUpdate;

    public SerializedResponse(@NonNull Response response) {
        this.response = RESPONSE_TO_STRING_CONVERTER.convertToDatabaseColumn(response);
        this.lastUpdate = now();
    }

    public Response getResponse() {
        return RESPONSE_TO_STRING_CONVERTER.convertToEntityAttribute(response);
    }
}
