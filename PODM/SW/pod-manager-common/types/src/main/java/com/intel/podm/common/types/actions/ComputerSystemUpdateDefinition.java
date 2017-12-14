/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.common.types.actions;

import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem.TrustedModule;

import java.util.HashSet;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public class ComputerSystemUpdateDefinition {
    private String assetTag;
    private BootSourceMode bootSourceMode;
    private BootSourceType bootSourceType;
    private BootSourceState bootSourceState;
    private Boolean userModeEnabled;
    private Set<TrustedModule> trustedModules = new HashSet<>();

    public ComputerSystemUpdateDefinition() {
    }

    public ComputerSystemUpdateDefinition(BootSourceMode bootSourceMode, BootSourceType bootSourceType, BootSourceState bootSourceState) {
        this.bootSourceMode = bootSourceMode;
        this.bootSourceType = bootSourceType;
        this.bootSourceState = bootSourceState;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public void setBootSourceMode(BootSourceMode bootSourceMode) {
        this.bootSourceMode = bootSourceMode;
    }

    public BootSourceMode getBootSourceMode() {
        return bootSourceMode;
    }

    public BootSourceState getBootSourceState() {
        return bootSourceState;
    }

    public void setBootSourceState(BootSourceState bootSourceState) {
        this.bootSourceState = bootSourceState;
    }

    public BootSourceType getBootSourceType() {
        return bootSourceType;
    }

    public void setBootSourceType(BootSourceType bootSourceType) {
        this.bootSourceType = bootSourceType;
    }

    public Boolean getUserModeEnabled() {
        return userModeEnabled;
    }

    public void setUserModeEnabled(Boolean userModeEnabled) {
        this.userModeEnabled = userModeEnabled;
    }

    public Set<TrustedModule> getTrustedModules() {
        return trustedModules;
    }

    public void setTrustedModules(Set<TrustedModule> trustedModules) {
        this.trustedModules = trustedModules;
    }
}
