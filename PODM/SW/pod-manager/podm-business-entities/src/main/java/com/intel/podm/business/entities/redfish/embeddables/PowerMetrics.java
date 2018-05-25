/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;

@Embeddable
public class PowerMetrics {
    @Column(name = "interval_in_min")
    private Integer intervalInMin;

    @Column(name = "min_consumed_watts")
    private BigDecimal minConsumedWatts;

    @Column(name = "max_consumed_watts")
    private BigDecimal maxConsumedWatts;

    @Column(name = "average_consumed_watts")
    private BigDecimal averageConsumedWatts;

    public Integer getIntervalInMin() {
        return intervalInMin;
    }

    public void setIntervalInMin(Integer intervalInMin) {
        this.intervalInMin = intervalInMin;
    }

    public BigDecimal getMinConsumedWatts() {
        return minConsumedWatts;
    }

    public void setMinConsumedWatts(BigDecimal minConsumedWatts) {
        this.minConsumedWatts = minConsumedWatts;
    }

    public BigDecimal getMaxConsumedWatts() {
        return maxConsumedWatts;
    }

    public void setMaxConsumedWatts(BigDecimal maxConsumedWatts) {
        this.maxConsumedWatts = maxConsumedWatts;
    }

    public BigDecimal getAverageConsumedWatts() {
        return averageConsumedWatts;
    }

    public void setAverageConsumedWatts(BigDecimal averageConsumedWatts) {
        this.averageConsumedWatts = averageConsumedWatts;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        PowerMetrics that = (PowerMetrics) o;

        return new EqualsBuilder()
            .append(intervalInMin, that.intervalInMin)
            .append(minConsumedWatts, that.minConsumedWatts)
            .append(maxConsumedWatts, that.maxConsumedWatts)
            .append(averageConsumedWatts, that.averageConsumedWatts)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(intervalInMin)
            .append(minConsumedWatts)
            .append(maxConsumedWatts)
            .append(averageConsumedWatts)
            .toHashCode();
    }
}
