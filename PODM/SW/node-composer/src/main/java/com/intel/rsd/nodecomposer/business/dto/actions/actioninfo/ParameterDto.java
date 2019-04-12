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

package com.intel.rsd.nodecomposer.business.dto.actions.actioninfo;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.rsd.nodecomposer.types.actions.ParameterDataType;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "name", "required", "dataType", "objectDataType", "allowableValues"
})
public class ParameterDto {
    private String name;
    private Boolean required;
    private ParameterDataType dataType;
    private String objectDataType;
    private Set<Object> allowableValues = new HashSet<>();

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Boolean getRequired() {
        return required;
    }

    public void setRequired(Boolean required) {
        this.required = required;
    }

    public ParameterDataType getDataType() {
        return dataType;
    }

    public void setDataType(ParameterDataType dataType) {
        this.dataType = dataType;
    }

    public String getObjectDataType() {
        return objectDataType;
    }

    public void setObjectDataType(String objectDataType) {
        this.objectDataType = objectDataType;
    }

    public Set<Object> getAllowableValues() {
        return allowableValues;
    }

    public void setAllowableValues(Set<Object> allowableValues) {
        this.allowableValues = allowableValues;
    }

    public void addAllowableValues(Set<?> allowableValues) {
        this.allowableValues.addAll(allowableValues);
    }
}
