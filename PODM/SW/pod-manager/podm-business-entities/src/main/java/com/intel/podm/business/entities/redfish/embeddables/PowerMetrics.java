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

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

@Embeddable
public class PowerMetrics {
    @Column(name = "interval_in_min")
    private Integer intervalInMin;

    @Column(name = "min_consumed_watts")
    private Integer minConsumedWatts;

    @Column(name = "max_consumed_watts")
    private Integer maxConsumedWatts;

    @Column(name = "average_consumed_watts")
    private Integer averageConsumedWatts;

    public Integer getIntervalInMin() {
        return intervalInMin;
    }

    public void setIntervalInMin(Integer intervalInMin) {
        this.intervalInMin = intervalInMin;
    }

    public Integer getMinConsumedWatts() {
        return minConsumedWatts;
    }

    public void setMinConsumedWatts(Integer minConsumedWatts) {
        this.minConsumedWatts = minConsumedWatts;
    }

    public Integer getMaxConsumedWatts() {
        return maxConsumedWatts;
    }

    public void setMaxConsumedWatts(Integer maxConsumedWatts) {
        this.maxConsumedWatts = maxConsumedWatts;
    }

    public Integer getAverageConsumedWatts() {
        return averageConsumedWatts;
    }

    public void setAverageConsumedWatts(Integer averageConsumedWatts) {
        this.averageConsumedWatts = averageConsumedWatts;
    }

    @Override
    @SuppressWarnings("checkstyle:CyclomaticComplexity")
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        PowerMetrics that = (PowerMetrics) o;
        return Objects.equals(intervalInMin, that.intervalInMin)
                && Objects.equals(minConsumedWatts, that.minConsumedWatts)
                && Objects.equals(maxConsumedWatts, that.maxConsumedWatts)
                && Objects.equals(averageConsumedWatts, that.averageConsumedWatts);
    }

    @Override
    public int hashCode() {
        return Objects.hash(intervalInMin, minConsumedWatts, maxConsumedWatts, averageConsumedWatts);
    }
}
