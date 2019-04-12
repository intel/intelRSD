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

package com.intel.rsd.nodecomposer.mappers.redfish;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.BootObject;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ComputerSystemResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.PerformanceConfigurationObject;
import com.intel.rsd.nodecomposer.mappers.EntityMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.SimpleTypeMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.TrustedModuleMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Boot;
import com.intel.rsd.nodecomposer.types.PerformanceConfigurationType;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Set;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class ComputerSystemMapper extends EntityMapper<ComputerSystemResource, ComputerSystem> {
    private final SimpleTypeMapper simpleTypeMapper;
    private final TrustedModuleMapper trustedModuleMapper;

    @Autowired
    public ComputerSystemMapper(SimpleTypeMapper simpleTypeMapper, TrustedModuleMapper trustedModuleMapper) {
        super(ComputerSystemResource.class, ComputerSystem.class);
        this.simpleTypeMapper = simpleTypeMapper;
        this.trustedModuleMapper = trustedModuleMapper;
    }

    @Override
    protected void performNotAutomatedMapping(ComputerSystemResource sourceComputerSystem, ComputerSystem targetComputerSystem) {
        super.performNotAutomatedMapping(source, target);
        sourceComputerSystem.getAllowableResetTypes().ifAssigned(allowableResetTypes ->
            simpleTypeMapper.map(allowableResetTypes, targetComputerSystem.getAllowableResetTypes(), targetComputerSystem::addAllowableResetType)
        );
        sourceComputerSystem.getPcieConnectionId().ifAssigned(pcieConnectionId ->
            simpleTypeMapper.map(pcieConnectionId, targetComputerSystem.getPcieConnectionIds(), targetComputerSystem::addPcieConnectionId)
        );
        sourceComputerSystem.getTrustedModules().ifAssigned(trustedModules ->
            trustedModuleMapper.map(trustedModules, targetComputerSystem.getTrustedModules(), targetComputerSystem::addTrustedModule)
        );
        mapPerformanceConfiguration(sourceComputerSystem, targetComputerSystem);
        mapBootProperty(sourceComputerSystem, targetComputerSystem);
    }

    private void mapPerformanceConfiguration(ComputerSystemResource sourceComputerSystem, ComputerSystem targetComputerSystem) {
        final ComputerSystemResource.Oem.RackScaleOem.PerformanceConfiguration performanceConfiguration = sourceComputerSystem.getPerformanceConfiguration();
        if (performanceConfiguration != null) {
            final Set<PerformanceConfigurationType> configurationTypes = performanceConfiguration.getConfigurations().stream()
                .map(PerformanceConfigurationObject::getType)
                .collect(toSet());
            simpleTypeMapper.map(configurationTypes, targetComputerSystem.getConfigurationTypes(), targetComputerSystem::addConfigurationType);
        }
    }

    private void mapBootProperty(ComputerSystemResource sourceComputerSystem, ComputerSystem targetComputerSystem) {
        BootObject sourceBoot = sourceComputerSystem.getBootObject();
        if (sourceBoot == null) {
            targetComputerSystem.setBoot(null);
            return;
        }

        Boot updatedBoot = mapBootObjects(sourceBoot, targetComputerSystem.getBoot());
        targetComputerSystem.setBoot(updatedBoot);
    }

    private Boot mapBootObjects(BootObject sourceBoot, Boot targetBoot) {
        Boot updatedBoot = ofNullable(targetBoot).orElseGet(Boot::new);

        sourceBoot.getBootSourceOverrideEnabled().ifAssigned(updatedBoot::setBootSourceOverrideEnabled);
        sourceBoot.getBootSourceOverrideMode().ifAssigned(updatedBoot::setBootSourceOverrideMode);
        sourceBoot.getBootSourceOverrideTarget().ifAssigned(updatedBoot::setBootSourceOverrideTarget);

        simpleTypeMapper.map(sourceBoot.getBootSourceOverrideModeAllowableValues(), updatedBoot.getBootSourceOverrideModeAllowableValues(),
            updatedBoot::addBootSourceOverrideModeAllowableValue);
        simpleTypeMapper.map(sourceBoot.getBootSourceOverrideTargetAllowableValues(), updatedBoot.getBootSourceOverrideTargetAllowableValues(),
            updatedBoot::addBootSourceOverrideTargetAllowableValue);

        return updatedBoot;
    }
}
