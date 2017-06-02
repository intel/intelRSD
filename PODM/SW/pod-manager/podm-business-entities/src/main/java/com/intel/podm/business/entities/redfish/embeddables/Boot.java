/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.OrderColumn;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class Boot {
    @Column(name = "boot_source_override_enabled")
    @Enumerated(STRING)
    private BootSourceState bootSourceOverrideEnabled;

    @Column(name = "boot_source_override_target")
    @Enumerated(STRING)
    private BootSourceType bootSourceOverrideTarget;

    @Column(name = "boot_source_override_mode")
    @Enumerated(STRING)
    private BootSourceMode bootSourceOverrideMode;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "boot_boot_source_override_supported", joinColumns = @JoinColumn(name = "boot_id"))
    @Column(name = "boot_source_override_supported")
    @OrderColumn(name = "boot_source_override_supported_order")
    private List<BootSourceType> bootSourceOverrideSupported = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "boot_boot_source_override_mode_supported", joinColumns = @JoinColumn(name = "boot_id"))
    @Column(name = "boot_source_override_mode_supported")
    @OrderColumn(name = "boot_source_override_mode_supported_order")
    private List<BootSourceMode> bootSourceOverrideModeSupported = new ArrayList<>();

    public BootSourceState getBootSourceOverrideEnabled() {
        return bootSourceOverrideEnabled;
    }

    public void setBootSourceOverrideEnabled(BootSourceState bootSourceOverrideEnabled) {
        this.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
    }

    public BootSourceType getBootSourceOverrideTarget() {
        return bootSourceOverrideTarget;
    }

    public void setBootSourceOverrideTarget(BootSourceType bootSourceOverrideTarget) {
        this.bootSourceOverrideTarget = bootSourceOverrideTarget;
    }

    public BootSourceMode getBootSourceOverrideMode() {
        return bootSourceOverrideMode;
    }

    public void setBootSourceOverrideMode(BootSourceMode bootSourceOverrideMode) {
        this.bootSourceOverrideMode = bootSourceOverrideMode;
    }

    public List<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
        return bootSourceOverrideSupported;
    }

    public void addBootSourceOverrideTargetAllowableValue(BootSourceType bootSourceOverrideSupported) {
        this.bootSourceOverrideSupported.add(bootSourceOverrideSupported);
    }

    public List<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
        return bootSourceOverrideModeSupported;
    }

    public void addBootSourceOverrideModeAllowableValue(BootSourceMode bootSourceOverrideModeSupported) {
        this.bootSourceOverrideModeSupported.add(bootSourceOverrideModeSupported);
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
        Boot boot = (Boot) o;
        return bootSourceOverrideEnabled == boot.bootSourceOverrideEnabled
            && bootSourceOverrideTarget == boot.bootSourceOverrideTarget
            && bootSourceOverrideMode == boot.bootSourceOverrideMode
            && Objects.equals(bootSourceOverrideSupported, boot.bootSourceOverrideSupported)
            && Objects.equals(bootSourceOverrideModeSupported, boot.bootSourceOverrideModeSupported);
    }

    @Override
    public int hashCode() {
        return Objects.hash(bootSourceOverrideEnabled, bootSourceOverrideTarget,
            bootSourceOverrideMode, bootSourceOverrideSupported,
            bootSourceOverrideModeSupported);
    }
}
