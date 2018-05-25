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
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;

@Embeddable
public class DriveMetricsHealthData {
    @Column(name = "available_spare_percentage")
    private BigDecimal availableSparePercentage;

    @Column(name = "predicted_media_life_used_percent")
    private BigDecimal predictedMediaLifeUsedPercent;

    @Column(name = "unsafe_shutdowns")
    private BigDecimal unsafeShutdowns;

    @Column(name = "media_errors")
    private BigDecimal mediaErrors;

    public BigDecimal getAvailableSparePercentage() {
        return availableSparePercentage;
    }

    public void setAvailableSparePercentage(BigDecimal availableSparePercentage) {
        this.availableSparePercentage = availableSparePercentage;
    }

    public BigDecimal getPredictedMediaLifeUsedPercent() {
        return predictedMediaLifeUsedPercent;
    }

    public void setPredictedMediaLifeUsedPercent(BigDecimal predictedMediaLifeUsedPercent) {
        this.predictedMediaLifeUsedPercent = predictedMediaLifeUsedPercent;
    }

    public BigDecimal getUnsafeShutdowns() {
        return unsafeShutdowns;
    }

    public void setUnsafeShutdowns(BigDecimal unsafeShutdowns) {
        this.unsafeShutdowns = unsafeShutdowns;
    }

    public BigDecimal getMediaErrors() {
        return mediaErrors;
    }

    public void setMediaErrors(BigDecimal mediaErrors) {
        this.mediaErrors = mediaErrors;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        DriveMetricsHealthData that = (DriveMetricsHealthData) o;
        return new EqualsBuilder()
            .append(availableSparePercentage, that.availableSparePercentage)
            .append(predictedMediaLifeUsedPercent, that.predictedMediaLifeUsedPercent)
            .append(unsafeShutdowns, that.unsafeShutdowns)
            .append(mediaErrors, that.mediaErrors)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(availableSparePercentage)
            .append(predictedMediaLifeUsedPercent)
            .append(unsafeShutdowns)
            .append(mediaErrors)
            .toHashCode();
    }
}
