/*
 * Copyright (c) 2016-2018 Intel Corporation
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
@SuppressWarnings("checkstyle:MethodCount")
public class ProcessorId {
    @Column(name = "vendor_id")
    private String vendorId;

    @Column(name = "identification_registers")
    private String identificationRegisters;

    @Column(name = "effective_family")
    private String effectiveFamily;

    @Column(name = "effective_model")
    private String effectiveModel;

    @Column(name = "step")
    private String step;

    @Column(name = "microcode_info")
    private String microcodeInfo;

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getIdentificationRegisters() {
        return identificationRegisters;
    }

    public void setIdentificationRegisters(String identificationRegisters) {
        this.identificationRegisters = identificationRegisters;
    }

    public String getEffectiveFamily() {
        return effectiveFamily;
    }

    public void setEffectiveFamily(String effectiveFamily) {
        this.effectiveFamily = effectiveFamily;
    }

    public String getEffectiveModel() {
        return effectiveModel;
    }

    public void setEffectiveModel(String effectiveModel) {
        this.effectiveModel = effectiveModel;
    }

    public String getStep() {
        return step;
    }

    public void setStep(String step) {
        this.step = step;
    }

    public String getMicrocodeInfo() {
        return microcodeInfo;
    }

    public void setMicrocodeInfo(String microcodeInfo) {
        this.microcodeInfo = microcodeInfo;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        ProcessorId that = (ProcessorId) o;

        return new EqualsBuilder()
            .append(vendorId, that.vendorId)
            .append(identificationRegisters, that.identificationRegisters)
            .append(effectiveFamily, that.effectiveFamily)
            .append(effectiveModel, that.effectiveModel)
            .append(step, that.step)
            .append(microcodeInfo, that.microcodeInfo)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(vendorId)
            .append(identificationRegisters)
            .append(effectiveFamily)
            .append(effectiveModel)
            .append(step)
            .append(microcodeInfo)
            .toHashCode();
    }
}
