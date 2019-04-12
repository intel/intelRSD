/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.MemoryModule;
import com.intel.rsd.nodecomposer.types.MemoryDeviceType;
import com.intel.rsd.nodecomposer.types.MemoryType;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "memory")
public class Memory extends DiscoverableEntity implements MemoryModule {
    @Getter
    @Setter
    @Column(name = "memory_device_type")
    @Enumerated(STRING)
    private MemoryDeviceType memoryDeviceType;

    @Getter
    @Setter
    @Column(name = "memory_type")
    @Enumerated(STRING)
    private MemoryType memoryType;

    @Getter
    @Setter
    @Column(name = "capacity_mib")
    private Integer capacityMib;

    @Getter
    @Setter
    @Column(name = "data_width_bits")
    private Integer dataWidthBits;

    @Getter
    @Setter
    @Column(name = "manufacturer")
    private String manufacturer;

    @Getter
    @Setter
    @Column(name = "operating_speed_mhz")
    private Integer operatingSpeedMhz;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !computerSystem.getMemoryModules().contains(this)) {
                computerSystem.addMemoryModule(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkMemoryModule(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkComputerSystem(computerSystem);
    }
}
