/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.dto.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({"target", "allowableValues"})
public class BindActionDto extends ActionDto {
    @JsonProperty("Port@Redfish.AllowableValues")
    private final Set<Context> allowableValues = new HashSet<>();

    public Set<Context> getAllowableValues() {
        return allowableValues;
    }
}
