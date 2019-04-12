/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish.embeddables;

import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.OrderColumn;
import java.util.ArrayList;
import java.util.List;

import static javax.persistence.EnumType.STRING;

@EqualsAndHashCode
@Embeddable
public class Boot {
    @Getter
    @Setter
    @Column(name = "boot_boot_source_override_enabled")
    @Enumerated(STRING)
    private BootSourceState bootSourceOverrideEnabled;

    @Getter
    @Setter
    @Column(name = "boot_boot_source_override_target")
    private String bootSourceOverrideTarget;

    @Getter
    @Setter
    @Column(name = "boot_boot_source_override_mode")
    @Enumerated(STRING)
    private BootSourceMode bootSourceOverrideMode;

    @Getter
    @ElementCollection
    @CollectionTable(name = "boot_boot_source_override_target_allowable_value", joinColumns = @JoinColumn(name = "boot_id"))
    @Column(name = "boot_source_override_target_allowable_value")
    @OrderColumn(name = "boot_source_override_target_allowable_value_order")
    private List<String> bootSourceOverrideTargetAllowableValues = new ArrayList<>();

    @Getter
    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "boot_boot_source_override_mode_allowable_value", joinColumns = @JoinColumn(name = "boot_id"))
    @Column(name = "boot_source_override_mode_allowable_value")
    @OrderColumn(name = "boot_source_override_mode_allowable_value_order")
    private List<BootSourceMode> bootSourceOverrideModeAllowableValues = new ArrayList<>();

    public void addBootSourceOverrideTargetAllowableValue(String bootSourceOverrideTargetAllowableValue) {
        this.bootSourceOverrideTargetAllowableValues.add(bootSourceOverrideTargetAllowableValue);
    }

    public void addBootSourceOverrideModeAllowableValue(BootSourceMode bootSourceOverrideModeAllowableValue) {
        this.bootSourceOverrideModeAllowableValues.add(bootSourceOverrideModeAllowableValue);
    }
}
