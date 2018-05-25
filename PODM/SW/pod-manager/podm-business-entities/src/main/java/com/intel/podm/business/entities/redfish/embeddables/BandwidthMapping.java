/*
 * Copyright (c) 2018 Intel Corporation
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

@Embeddable
public class BandwidthMapping {

    @Column(name = "priority_group")
    private Integer priorityGroup;

    @Column(name = "bandwidth_percent")
    private Integer bandwidthPercent;


    public Integer getPriorityGroup() {
        return priorityGroup;
    }

    public void setPriorityGroup(Integer trafficClass) {
        this.priorityGroup = trafficClass;
    }

    public Integer getBandwidthPercent() {
        return bandwidthPercent;
    }

    public void setBandwidthPercent(Integer bandwidthPrecent) {
        this.bandwidthPercent = bandwidthPrecent;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        BandwidthMapping that = (BandwidthMapping) o;

        return new EqualsBuilder()
            .append(priorityGroup, that.priorityGroup)
            .append(bandwidthPercent, that.bandwidthPercent)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(priorityGroup)
            .append(bandwidthPercent)
            .toHashCode();
    }
}
