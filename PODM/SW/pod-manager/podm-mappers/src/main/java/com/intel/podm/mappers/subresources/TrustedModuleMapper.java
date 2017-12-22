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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class TrustedModuleMapper extends SubresourceMapper<RedfishComputerSystem.TrustedModule, TrustedModule> {
    @Inject
    public TrustedModuleMapper(EmbeddableCleanAndCreateStrategy<TrustedModule> embeddableCleanAndCreateStrategy) {
        super(TrustedModule.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(RedfishComputerSystem.TrustedModule sourceModule, TrustedModule targetModule) {
        sourceModule.getFirmwareVersion().ifAssigned(targetModule::setFirmwareVersion);
        sourceModule.getFirmwareVersion2().ifAssigned(targetModule::setFirmwareVersion2);
        sourceModule.getInterfaceType().ifAssigned(targetModule::setInterfaceType);
        sourceModule.getInterfaceTypeSelection().ifAssigned(targetModule::setInterfaceTypeSelection);
        sourceModule.getStatus().ifAssigned(targetModule::setStatus);
    }

    @Override
    public boolean equals(RedfishComputerSystem.TrustedModule sourceModule, TrustedModule targetModule) {
        return Objects.equals(sourceModule.getFirmwareVersion().orElse(targetModule.getFirmwareVersion()), targetModule.getFirmwareVersion())
            && Objects.equals(sourceModule.getInterfaceType().orElse(targetModule.getInterfaceType()), targetModule.getInterfaceType())
            && Objects.equals(sourceModule.getStatus().orElse(targetModule.getStatus()), targetModule.getStatus());
    }
}
