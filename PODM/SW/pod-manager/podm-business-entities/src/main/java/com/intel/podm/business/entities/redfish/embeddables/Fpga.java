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

import com.intel.podm.common.types.FpgaType;
import com.intel.podm.common.types.HssiConfig;
import com.intel.podm.common.types.HssiSideband;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

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
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Fpga fpga = (Fpga) o;

        return new EqualsBuilder()
            .append(reconfigurationSlots, fpga.reconfigurationSlots)
            .append(type, fpga.type)
            .append(bitStreamVersion, fpga.bitStreamVersion)
            .append(hssiConfiguration, fpga.hssiConfiguration)
            .append(hssiSideband, fpga.hssiSideband)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(type)
            .append(bitStreamVersion)
            .append(hssiConfiguration)
            .append(hssiSideband)
            .append(reconfigurationSlots)
            .toHashCode();
    }
}
