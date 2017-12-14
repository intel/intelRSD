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

import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.InterfaceTypeSelection;
import com.intel.podm.common.types.Status;
import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class TrustedModule {
    @Column(name = "firmware_version")
    private String firmwareVersion;

    @Column(name = "firmware_version_2")
    private String firmwareVersion2;

    @Column(name = "interface_type")
    @Enumerated(STRING)
    private InterfaceType interfaceType;

    @Column(name = "interface_type_selection")
    @Enumerated(STRING)
    private InterfaceTypeSelection interfaceTypeSelection;

    @Column(name = "status")
    private Status status;

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public String getFirmwareVersion2() {
        return firmwareVersion2;
    }

    public void setFirmwareVersion2(String firmwareVersion2) {
        this.firmwareVersion2 = firmwareVersion2;
    }

    public InterfaceType getInterfaceType() {
        return interfaceType;
    }

    public void setInterfaceType(InterfaceType interfaceType) {
        this.interfaceType = interfaceType;
    }

    public InterfaceTypeSelection getInterfaceTypeSelection() {
        return interfaceTypeSelection;
    }

    public void setInterfaceTypeSelection(InterfaceTypeSelection interfaceTypeSelection) {
        this.interfaceTypeSelection = interfaceTypeSelection;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        TrustedModule that = (TrustedModule) o;
        return new EqualsBuilder()
            .append(firmwareVersion, that.firmwareVersion)
            .append(firmwareVersion2, that.firmwareVersion2)
            .append(interfaceType, that.interfaceType)
            .append(interfaceTypeSelection, that.interfaceTypeSelection)
            .append(status, that.status)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(firmwareVersion, firmwareVersion2, interfaceType, status, interfaceTypeSelection);
    }
}
