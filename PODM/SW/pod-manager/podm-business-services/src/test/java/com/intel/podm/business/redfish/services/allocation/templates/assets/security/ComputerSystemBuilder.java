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

package com.intel.podm.business.redfish.services.allocation.templates.assets.security;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.common.types.InterfaceType;

import java.util.ArrayList;
import java.util.List;

import static java.util.Arrays.asList;

public final class ComputerSystemBuilder {
    private boolean txtEnabled;
    private List<InterfaceType> interfaceTypes = new ArrayList<>();

    private ComputerSystemBuilder() {
    }

    public static ComputerSystemBuilder systemInTestBuilder() {
        return new ComputerSystemBuilder();
    }

    public ComputerSystemBuilder txtEnabled(boolean txtEnabled) {
        this.txtEnabled = txtEnabled;
        return this;
    }

    public ComputerSystemBuilder interfaceTypes(InterfaceType... interfaceType) {
        this.interfaceTypes.addAll(asList(interfaceType));
        return this;
    }

    public ComputerSystem build() {
        ComputerSystem computerSystem = new ComputerSystem();
        computerSystem.setTrustedExecutionTechnologyEnabled(txtEnabled);
        for (InterfaceType interfaceType : interfaceTypes) {
            TrustedModule trustedModule = new TrustedModule();
            trustedModule.setInterfaceType(interfaceType);
            computerSystem.addTrustedModule(trustedModule);
        }
        return computerSystem;
    }
}
