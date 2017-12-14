/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import java.util.Objects;

@Embeddable
public class CalculationParamsType {
    @Column(name = "source_metric")
    private String sourceMetric;

    @Column(name = "result_metric")
    private String resultMetric;


    public String getResultMetric() {
        return resultMetric;
    }

    public void setResultMetric(String resultMetric) {
        this.resultMetric = resultMetric;
    }

    public String getSourceMetric() {
        return sourceMetric;
    }

    public void setSourceMetric(String sourceMetric) {
        this.sourceMetric = sourceMetric;
    }


    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof CalculationParamsType)) {
            return false;
        }

        CalculationParamsType that = (CalculationParamsType) o;

        return new EqualsBuilder()
            .append(getSourceMetric(), that.getSourceMetric())
            .append(getResultMetric(), that.getResultMetric())
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(sourceMetric, resultMetric);
    }
}
