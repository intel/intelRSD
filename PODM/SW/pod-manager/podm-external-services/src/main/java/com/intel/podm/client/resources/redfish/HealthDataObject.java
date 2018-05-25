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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.math.BigDecimal;

public class HealthDataObject {
    @JsonProperty("RemainingSpareBlockPercentage")
    private BigDecimal remainingSpareBlockPercentage;

    @JsonProperty("LastShutdownSuccess")
    private Boolean lastShutdownSuccess;

    @JsonProperty("DataLossDetected")
    private Boolean dataLossDetected;

    @JsonProperty("PerformanceDegraded")
    private Boolean performanceDegraded;

    @JsonProperty("AlarmTrips")
    private AlarmTripsObject alarmTrips;

    public BigDecimal getRemainingSpareBlockPercentage() {
        return remainingSpareBlockPercentage;
    }

    public Boolean getLastShutdownSuccess() {
        return lastShutdownSuccess;
    }

    public Boolean getDataLossDetected() {
        return dataLossDetected;
    }

    public Boolean getPerformanceDegraded() {
        return performanceDegraded;
    }

    public AlarmTripsObject getAlarmTrips() {
        return alarmTrips;
    }
}
