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

import com.intel.podm.common.types.FpgaType;
import com.intel.podm.common.types.HssiConfig;
import com.intel.podm.common.types.HssiSideband;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class Fpga {
    @Column(name = "type")
    @Enumerated(STRING)
    private FpgaType type;

    @Column(name = "bit_stream_version")
    private String bitStreamVersion;

    @Column(name = "hssi_configuration")
    @Enumerated(STRING)
    private HssiConfig hssiConfiguration;

    @Column(name = "hssi_sideband")
    @Enumerated(STRING)
    private HssiSideband hssiSideband;

    @Column(name = "reconfiguration_slots")
    private int reconfigurationSlots;

    public FpgaType getType() {
        return type;
    }

    public void setType(FpgaType type) {
        this.type = type;
    }

    public String getBitStreamVersion() {
        return bitStreamVersion;
    }

    public void setBitStreamVersion(String bitStreamVersion) {
        this.bitStreamVersion = bitStreamVersion;
    }

    public HssiConfig getHssiConfiguration() {
        return hssiConfiguration;
    }

    public void setHssiConfiguration(HssiConfig hssiConfiguration) {
        this.hssiConfiguration = hssiConfiguration;
    }

    public HssiSideband getHssiSideband() {
        return hssiSideband;
    }

    public void setHssiSideband(HssiSideband hssiSideband) {
        this.hssiSideband = hssiSideband;
    }

    public int getReconfigurationSlots() {
        return reconfigurationSlots;
    }

    public void setReconfigurationSlots(int reconfigurationSlots) {
        this.reconfigurationSlots = reconfigurationSlots;
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
        Fpga that = (Fpga) o;
        return Objects.equals(type, that.type)
            && Objects.equals(bitStreamVersion, that.bitStreamVersion)
            && Objects.equals(hssiConfiguration, that.hssiConfiguration)
            && Objects.equals(hssiSideband, that.hssiSideband)
            && Objects.equals(reconfigurationSlots, that.reconfigurationSlots);
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, bitStreamVersion, hssiConfiguration, hssiSideband, reconfigurationSlots);
    }
}
