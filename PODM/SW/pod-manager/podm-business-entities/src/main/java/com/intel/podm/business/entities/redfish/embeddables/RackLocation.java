/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.common.types.rmm.RackUnitType;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class RackLocation {
    @Column(name = "x_location")
    private Integer xLocation;

    @Column(name = "u_location")
    private Integer uLocation;

    @Column(name = "u_height")
    private Integer uHeight;

    @Column(name = "rack_unit")
    @Enumerated(STRING)
    private RackUnitType rackUnits;

    public Integer getXLocation() {
        return xLocation;
    }

    public void setXLocation(Integer xLocation) {
        this.xLocation = xLocation;
    }

    public Integer getULocation() {
        return uLocation;
    }

    public void setULocation(Integer uLocation) {
        this.uLocation = uLocation;
    }

    public Integer getUHeight() {
        return uHeight;
    }

    public void setUHeight(Integer uHeight) {
        this.uHeight = uHeight;
    }

    public RackUnitType getRackUnits() {
        return rackUnits;
    }

    public void setRackUnits(RackUnitType rackUnit) {
        this.rackUnits = rackUnit;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        RackLocation that = (RackLocation) o;

        return new EqualsBuilder()
            .append(xLocation, that.xLocation)
            .append(uLocation, that.uLocation)
            .append(uHeight, that.uHeight)
            .append(rackUnits, that.rackUnits)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(xLocation)
            .append(uLocation)
            .append(uHeight)
            .append(rackUnits)
            .toHashCode();
    }
}
