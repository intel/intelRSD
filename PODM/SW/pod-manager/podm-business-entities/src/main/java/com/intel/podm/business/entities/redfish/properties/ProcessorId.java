/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ProcessorId extends DomainObject {
    public static final DomainObjectProperty<String> VENDOR_ID = stringProperty("vendorId");
    public static final DomainObjectProperty<String> IDENTIFICATION_REGISTERS = stringProperty("identificationRegisters");
    public static final DomainObjectProperty<String> EFFECTIVE_FAMILY = stringProperty("effectiveFamily");
    public static final DomainObjectProperty<String> EFFECTIVE_MODEL = stringProperty("effectiveModel");
    public static final DomainObjectProperty<String> STEP = stringProperty("step");
    public static final DomainObjectProperty<String> MICROCODE_INFO = stringProperty("microcodeInfo");

    public String getVendorId() {
        return getProperty(VENDOR_ID);
    }

    public void setVendorId(String vendorId) {
        setProperty(VENDOR_ID, vendorId);
    }

    public String getIdentificationRegisters() {
        return getProperty(IDENTIFICATION_REGISTERS);
    }

    public void setIdentificationRegisters(String identificationRegisters) {
        setProperty(IDENTIFICATION_REGISTERS, identificationRegisters);
    }

    public String getEffectiveFamily() {
        return getProperty(EFFECTIVE_FAMILY);
    }

    public void setEffectiveFamily(String effectiveFamily) {
        setProperty(EFFECTIVE_FAMILY, effectiveFamily);
    }

    public String getEffectiveModel() {
        return getProperty(EFFECTIVE_MODEL);
    }

    public void setEffectiveModel(String effectiveModel) {
        setProperty(EFFECTIVE_MODEL, effectiveModel);
    }

    public String getStep() {
        return getProperty(STEP);
    }

    public void setStep(String step) {
        setProperty(STEP, step);
    }

    public String getMicrocodeInfo() {
        return getProperty(MICROCODE_INFO);
    }

    public void setMicrocodeInfo(String microcodeInfo) {
        setProperty(MICROCODE_INFO, microcodeInfo);
    }
}
