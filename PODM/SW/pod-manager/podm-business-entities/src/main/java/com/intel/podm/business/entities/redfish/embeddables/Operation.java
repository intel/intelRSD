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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;
import java.util.Objects;

@Embeddable
public class Operation {
    @Column(name = "operation_name")
    private String operationName;

    @Column(name = "percentage_complete")
    private BigDecimal percentageComplete;

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

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Operation that = (Operation) o;

        return new EqualsBuilder()
            .append(operationName, that.operationName)
            .append(percentageComplete, that.percentageComplete)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(operationName, percentageComplete);
    }
}
