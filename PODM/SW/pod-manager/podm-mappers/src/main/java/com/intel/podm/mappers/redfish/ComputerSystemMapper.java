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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.client.resources.redfish.BootObject;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.ComputerSystemDeviceMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;
import com.intel.podm.mappers.subresources.TrustedModuleMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.common.types.DeepDiscoveryState.DONE;
import static com.intel.podm.common.types.DeepDiscoveryState.INITIAL;
import static com.intel.podm.common.types.DiscoveryState.BASIC;
import static com.intel.podm.common.types.DiscoveryState.DEEP;
import static java.util.Optional.ofNullable;

@Dependent
public class ComputerSystemMapper extends EntityMapper<ComputerSystemResource, ComputerSystem> {
    @Inject
    ComputerSystemDeviceMapper computerSystemDeviceMapper;

    @Inject
    SimpleTypeMapper simpleTypeMapper;

    @Inject
    TrustedModuleMapper trustedModuleMapper;

    public ComputerSystemMapper() {
        super(ComputerSystemResource.class, ComputerSystem.class);
    }

    @Override
    protected void performNotAutomatedMapping(ComputerSystemResource sourceComputerSystem, ComputerSystem targetComputerSystem) {
        super.performNotAutomatedMapping(source, target);
        sourceComputerSystem.getPciDevices().ifAssigned(pciDevices ->
            computerSystemDeviceMapper.map(pciDevices, targetComputerSystem.getPciDevices(), targetComputerSystem::addPciDevice)
        );
        sourceComputerSystem.getAllowableResetTypes().ifAssigned(allowableResetTypes ->
            simpleTypeMapper.map(allowableResetTypes, targetComputerSystem.getAllowableResetTypes(), targetComputerSystem::addAllowableResetType)
        );
        sourceComputerSystem.getAllowableInterfaceTypes().ifAssigned(allowableInterfaceTypes ->
            simpleTypeMapper.map(allowableInterfaceTypes, targetComputerSystem.getAllowableInterfaceTypes(), targetComputerSystem::addAllowableInterfaceType)
        );
        sourceComputerSystem.getPcieConnectionId().ifAssigned(pcieConnectionId ->
            simpleTypeMapper.map(pcieConnectionId, targetComputerSystem.getPcieConnectionIds(), targetComputerSystem::addPcieConnectionId)
        );
        sourceComputerSystem.getTrustedModules().ifAssigned(trustedModules ->
            trustedModuleMapper.map(trustedModules, targetComputerSystem.getTrustedModules(), targetComputerSystem::addTrustedModule)
        );
        mapBootProperty(sourceComputerSystem, targetComputerSystem);
        setProperDiscoveryState(sourceComputerSystem, targetComputerSystem);
    }

    private void setProperDiscoveryState(ComputerSystemResource sourceComputerSystem, ComputerSystem targetComputerSystem) {
        DiscoveryState discoveryState = targetComputerSystem.getDiscoveryState();

        if (!sourceComputerSystem.isBasic()) {
            targetComputerSystem.setDiscoveryState(DEEP);
            targetComputerSystem.getMetadata().setDeepDiscoveryState(DONE);
        } else if (discoveryState == null) {
            targetComputerSystem.setDiscoveryState(BASIC);
            targetComputerSystem.getMetadata().setDeepDiscoveryState(INITIAL);
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
