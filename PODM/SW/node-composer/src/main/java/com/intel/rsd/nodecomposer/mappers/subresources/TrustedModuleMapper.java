/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.subresources;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.TrustedModuleObject;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.TrustedModule;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class TrustedModuleMapper extends SubresourceMapper<TrustedModuleObject, TrustedModule> {
    @Autowired
    public TrustedModuleMapper(EmbeddableCleanAndCreateStrategy<TrustedModule> embeddableCleanAndCreateStrategy) {
        super(TrustedModule.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(TrustedModuleObject sourceModule, TrustedModule targetModule) {
        sourceModule.getFirmwareVersion().ifAssigned(targetModule::setFirmwareVersion);
        sourceModule.getInterfaceType().ifAssigned(targetModule::setInterfaceType);
        sourceModule.getStatus().ifAssigned(targetModule::setStatus);
    }

    @Override
    public boolean equals(TrustedModuleObject sourceModule, TrustedModule targetModule) {
        return Objects.equals(sourceModule.getFirmwareVersion().orElse(targetModule.getFirmwareVersion()), targetModule.getFirmwareVersion())
            && Objects.equals(sourceModule.getInterfaceType().orElse(targetModule.getInterfaceType()), targetModule.getInterfaceType())
            && Objects.equals(sourceModule.getStatus().orElse(targetModule.getStatus()), targetModule.getStatus());
    }
}
