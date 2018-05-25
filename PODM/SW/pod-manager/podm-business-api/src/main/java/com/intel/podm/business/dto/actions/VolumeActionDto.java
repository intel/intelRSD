/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.common.types.actions.InitializeType;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({"target", "allowableValues"})
public class VolumeActionDto extends ActionDto {
    @JsonProperty("InitializeType@Redfish.AllowableValues")
    private List<InitializeType> allowableValues = new ArrayList<>();

    public List<InitializeType> getAllowableValues() {
        return allowableValues;
    }

    public void setAllowableValues(List<InitializeType> allowableValues) {
        this.allowableValues = allowableValues;
    }
}
