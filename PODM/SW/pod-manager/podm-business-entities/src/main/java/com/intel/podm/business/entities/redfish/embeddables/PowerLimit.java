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

import com.intel.podm.common.types.PowerLimitException;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class PowerLimit {
    @Column(name = "limit_in_watts")
    private Integer limitInWatts;

    @Column(name = "limit_exception")
    @Enumerated(STRING)
    private PowerLimitException limitException;

    @Column(name = "correction_in_ms")
    private Integer correctionInMs;

    public Integer getLimitInWatts() {
        return limitInWatts;
    }

    public void setLimitInWatts(Integer limitInWatts) {
        this.limitInWatts = limitInWatts;
    }

    public PowerLimitException getLimitException() {
        return limitException;
    }

    public void setLimitException(PowerLimitException limitException) {
        this.limitException = limitException;
    }

    public Integer getCorrectionInMs() {
        return correctionInMs;
    }

    public void setCorrectionInMs(Integer correctionInMs) {
        this.correctionInMs = correctionInMs;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        PowerLimit that = (PowerLimit) o;
        return Objects.equals(limitInWatts, that.limitInWatts)
                && limitException == that.limitException
                && Objects.equals(correctionInMs, that.correctionInMs);
    }

    @Override
    public int hashCode() {
        return Objects.hash(limitInWatts, limitException, correctionInMs);
    }
}
