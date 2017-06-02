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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.client.api.resources.redfish.ManagerResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.Optional.ofNullable;

@Dependent
public class ManagerMapper extends EntityMapper<ManagerResource, Manager> {

    @Inject
    private SimpleTypeMapper simpleTypeMapper;

    public ManagerMapper() {
        super(ManagerResource.class, Manager.class);
        ignoredProperties("connectTypesSupported");
    }

    @Override
    protected void performNotAutomatedMapping(ManagerResource sourceManager, Manager targetManager) {
        super.performNotAutomatedMapping(sourceManager, targetManager);
        mapSerialConsole(source, target);
        mapGraphicalConsole(source, target);
        mapCommandShell(source, target);
    }

    private void mapCommandShell(ManagerResource source, Manager target) {
        ofNullable(source.getCommandShell()).ifPresent(console ->
            simpleTypeMapper.map(
                console.getConnectTypesSupported(),
                target.getCommandShell().getConnectTypesSupported(),
                target.getCommandShell()::addConnectTypesSupported
            )
        );
    }

    private void mapGraphicalConsole(ManagerResource source, Manager target) {
        ofNullable(source.getGraphicalConsole()).ifPresent(console ->
            simpleTypeMapper.map(
                console.getConnectTypesSupported(),
                target.getGraphicalConsole().getConnectTypesSupported(),
                target.getGraphicalConsole()::addConnectTypesSupported
            )
        );
    }

    private void mapSerialConsole(ManagerResource source, Manager target) {
        ofNullable(source.getSerialConsole()).ifPresent(console ->
            simpleTypeMapper.map(
                console.getConnectTypesSupported(),
                target.getSerialConsole().getConnectTypesSupported(),
                target.getSerialConsole()::addConnectTypesSupported
            )
        );
    }
}
