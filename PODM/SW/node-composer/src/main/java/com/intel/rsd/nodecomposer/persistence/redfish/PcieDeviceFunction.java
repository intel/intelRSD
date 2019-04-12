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

import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "pcie_device_function")
public class PcieDeviceFunction extends DiscoverableEntity {
    @OneToMany(mappedBy = "pcieDeviceFunction", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "pcie_device_id")
    private PcieDevice pcieDevice;

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getPcieDeviceFunction())) {
            drive.setPcieDeviceFunction(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkPcieDeviceFunction(this);
            }
        }
    }

    public PcieDevice getPcieDevice() {
        return pcieDevice;
    }

    public void setPcieDevice(PcieDevice pcieDevice) {
        if (!Objects.equals(this.pcieDevice, pcieDevice)) {
            unlinkPcieDevice(this.pcieDevice);
            this.pcieDevice = pcieDevice;
            if (pcieDevice != null && !pcieDevice.getPcieDeviceFunctions().contains(this)) {
                pcieDevice.addPcieDeviceFunction(this);
            }
        }
    }

    public void unlinkPcieDevice(PcieDevice pcieDevice) {
        if (Objects.equals(this.pcieDevice, pcieDevice)) {
            this.pcieDevice = null;
            if (pcieDevice != null) {
                pcieDevice.unlinkPcieDeviceFunction(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(drives, this::unlinkDrive);
        unlinkPcieDevice(pcieDevice);
    }
}
