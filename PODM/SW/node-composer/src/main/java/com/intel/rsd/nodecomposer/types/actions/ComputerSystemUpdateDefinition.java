/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.actions;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.TrustedModuleObject;
import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;

@Getter
@Setter
@NoArgsConstructor
public class ComputerSystemUpdateDefinition implements Serializable {
    private static final long serialVersionUID = 1757730184993744157L;

    private String assetTag;
    private BootSourceMode bootSourceMode;
    private String bootSourceType;
    private BootSourceState bootSourceState;
    private Boolean userModeEnabled;
    private Set<TrustedModuleObject> trustedModules = new HashSet<>();
}
