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

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"operationName", "percentageComplete", "associatedTask"})
@JsonInclude(NON_NULL)
public class OperationDto {
    private String operationName;
    private BigDecimal percentageComplete;
    private Set<Context> associatedTask = new HashSet<>();

    public String getOperationName() {
        return operationName;
    }

    public void setOperationName(String operationName) {
        this.operationName = operationName;
    }

    public BigDecimal getPercentageComplete() {
        return percentageComplete;
    }

    public void setPercentageComplete(BigDecimal percentageComplete) {
        this.percentageComplete = percentageComplete;
    }

    public Set<Context> getAssociatedTask() {
        return associatedTask;
    }

    public void setAssociatedTask(Set<Context> associatedTask) {
        this.associatedTask = associatedTask;
    }
}
