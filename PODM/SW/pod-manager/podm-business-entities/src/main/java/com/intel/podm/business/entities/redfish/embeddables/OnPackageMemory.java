/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.common.types.ProcessorMemoryType;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class OnPackageMemory {
    @Column(name = "type")
    @Enumerated(STRING)
    private ProcessorMemoryType type;
    @Column(name = "capacity_mb")
    private int capacityMb;
    @Column(name = "speed_mhz")
    private int speedMhz;

    public ProcessorMemoryType getType() {
        return type;
    }

    public void setType(ProcessorMemoryType type) {
        this.type = type;
    }

    public int getCapacityMb() {
        return capacityMb;
    }

    public void setCapacityMb(int capacityMb) {
        this.capacityMb = capacityMb;
    }

    public int getSpeedMhz() {
        return speedMhz;
    }

    public void setSpeedMhz(int speedMhz) {
        this.speedMhz = speedMhz;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        OnPackageMemory that = (OnPackageMemory) o;
        return Objects.equals(type, that.type)
            && Objects.equals(capacityMb, that.capacityMb)
            && Objects.equals(speedMhz, that.speedMhz);
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, capacityMb, speedMhz);
    }
}
