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
import javax.persistence.Embedded;
import java.math.BigDecimal;
import java.util.Objects;

@Embeddable
public class HealthData {
    @Column(name = "remaining_spare_block_percentage")
    private BigDecimal remainingSpareBlockPercentage;

    @Column(name = "last_shutdown_success")
    private Boolean lastShutdownSuccess;

    @Column(name = "data_loss_detected")
    private Boolean dataLossDetected;

    @Column(name = "performance_degraded")
    private Boolean performanceDegraded;

    @Embedded
    private AlarmTrips alarmTrips;

    public BigDecimal getRemainingSpareBlockPercentage() {
        return remainingSpareBlockPercentage;
    }

    public void setRemainingSpareBlockPercentage(BigDecimal remainingSpareBlockPercentage) {
        this.remainingSpareBlockPercentage = remainingSpareBlockPercentage;
    }

    public Boolean getLastShutdownSuccess() {
        return lastShutdownSuccess;
    }

    public void setLastShutdownSuccess(Boolean lastShutdownSuccess) {
        this.lastShutdownSuccess = lastShutdownSuccess;
    }

    public Boolean getDataLossDetected() {
        return dataLossDetected;
    }

    public void setDataLossDetected(Boolean dataLossDetected) {
        this.dataLossDetected = dataLossDetected;
    }

    public Boolean getPerformanceDegraded() {
        return performanceDegraded;
    }

    public void setPerformanceDegraded(Boolean performanceDegraded) {
        this.performanceDegraded = performanceDegraded;
    }

    public AlarmTrips getAlarmTrips() {
        return alarmTrips;
    }

    public void setAlarmTrips(AlarmTrips alarmTrips) {
        this.alarmTrips = alarmTrips;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        HealthData that = (HealthData) o;
        return new EqualsBuilder()
            .append(remainingSpareBlockPercentage, that.remainingSpareBlockPercentage)
            .append(lastShutdownSuccess, that.lastShutdownSuccess)
            .append(dataLossDetected, that.dataLossDetected)
            .append(performanceDegraded, that.performanceDegraded)
            .append(alarmTrips, that.alarmTrips)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(remainingSpareBlockPercentage, lastShutdownSuccess, dataLossDetected, performanceDegraded, alarmTrips);
    }
}
