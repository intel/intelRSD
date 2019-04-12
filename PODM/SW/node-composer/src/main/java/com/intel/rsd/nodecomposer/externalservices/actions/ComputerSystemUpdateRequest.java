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

package com.intel.rsd.nodecomposer.externalservices.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.TrustedModuleObject;
import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import com.intel.rsd.nodecomposer.types.InterfaceType;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;

import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_EMPTY;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static java.util.stream.Collectors.toSet;

@JsonInclude(NON_NULL)
public class ComputerSystemUpdateRequest {
    @JsonProperty("AssetTag")
    private String assetTag;

    @JsonProperty("Boot")
    private BootSourceOverride bootSourceOverride;

    @JsonProperty("TrustedModules")
    @JsonInclude(NON_EMPTY)
    private Set<TrustedModuleUpdateDefinition> trustedModules = new HashSet<>();

    @JsonProperty("Oem")
    private Oem oem;

    public ComputerSystemUpdateRequest(ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
        this.assetTag = computerSystemUpdateDefinition.getAssetTag();
        this.trustedModules.addAll(convertToTrustedModuleUpdateDefinition(computerSystemUpdateDefinition.getTrustedModules()));

        if (!allBootPropertiesAreNull(computerSystemUpdateDefinition)) {
            this.bootSourceOverride = new BootSourceOverride(computerSystemUpdateDefinition);
        }

       if (computerSystemUpdateDefinition.getUserModeEnabled() != null) {
            if (this.oem == null) {
                this.oem = new Oem();
            }
            this.oem.rackScaleOem.userModeEnabled = computerSystemUpdateDefinition.getUserModeEnabled();
        }
    }

    private boolean allBootPropertiesAreNull(ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
        return computerSystemUpdateDefinition.getBootSourceMode() == null
            && computerSystemUpdateDefinition.getBootSourceState() == null
            && computerSystemUpdateDefinition.getBootSourceType() == null;
    }

    private Set<TrustedModuleUpdateDefinition> convertToTrustedModuleUpdateDefinition(Set<TrustedModuleObject> trustedModules) {
        return trustedModules.stream()
            .map(TrustedModuleUpdateDefinition::new)
            .collect(toSet());
    }

    @JsonInclude(NON_NULL)
    private class BootSourceOverride {
        @JsonProperty("BootSourceOverrideEnabled")
        private BootSourceState bootSourceState;

        @JsonProperty("BootSourceOverrideTarget")
        private String bootSourceType;

        @JsonProperty("BootSourceOverrideMode")
        private BootSourceMode bootSourceMode;

        BootSourceOverride(ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
            bootSourceState = computerSystemUpdateDefinition.getBootSourceState();
            bootSourceType = computerSystemUpdateDefinition.getBootSourceType();
            bootSourceMode = computerSystemUpdateDefinition.getBootSourceMode();
        }
    }

    @JsonInclude(NON_NULL)
    private class Oem {
        @JsonProperty("Intel_RackScale")
        private ComputerSystemUpdateRequest.Oem.RackScaleOem rackScaleOem = new ComputerSystemUpdateRequest.Oem.RackScaleOem();

        @JsonInclude(NON_NULL)
        private class RackScaleOem {
            @JsonProperty("UserModeEnabled")
            private Boolean userModeEnabled;
        }
    }

    @JsonInclude(NON_NULL)
    private class TrustedModuleUpdateDefinition {
        @JsonProperty("FirmwareVersion")
        private String firmwareVersion;

        @JsonProperty("InterfaceType")
        private InterfaceType interfaceType;

        @JsonProperty("Status")
        private Status status;

        TrustedModuleUpdateDefinition(TrustedModuleObject trustedModule) {
            firmwareVersion = trustedModule.getFirmwareVersion().get();
            interfaceType = trustedModule.getInterfaceType().get();
            status = trustedModule.getStatus().get();
        }
    }
}
