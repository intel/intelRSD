/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.databind.JsonNode;
import org.springframework.lang.Nullable;
import org.springframework.validation.AbstractBindingResult;

public class PatchRequestBodyBindingResult extends AbstractBindingResult {
    private static final long serialVersionUID = -5569486459358422162L;
    private final JsonNode requestBody;

    PatchRequestBodyBindingResult(JsonNode requestBody, String objectName) {
        super(objectName);
        this.requestBody = requestBody;
    }

    @Override
    public Object getTarget() {
        return this.requestBody;
    }

    @Override
    protected Object getActualFieldValue(@Nullable String field) {
        return this.requestBody.at(field);
    }
}
