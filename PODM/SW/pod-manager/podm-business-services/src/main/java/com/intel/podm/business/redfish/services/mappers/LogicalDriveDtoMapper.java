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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.LogicalDriveDto;
import com.intel.podm.business.entities.redfish.LogicalDrive;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.ContextCollections.asLogicalDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPhysicalDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asRemoteTargetContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
class LogicalDriveDtoMapper extends DtoMapper<LogicalDrive, LogicalDriveDto> {
    LogicalDriveDtoMapper() {
        super(LogicalDrive.class, LogicalDriveDto.class);
        this.ignoredProperties("links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(LogicalDrive source, LogicalDriveDto target) {
        super.performNotAutomatedMapping(source, target);

        LogicalDriveDto.Links links = target.getLinks();
        links.getUsedBy().addAll(asLogicalDriveContexts(source.getUsedByLogicalDrives()));
        links.getPhysicalDrives().addAll(asPhysicalDriveContexts(source.getPhysicalDrives()));
        links.setMasterDrive(toContext(source.getMasterDrive()));
        links.getLogicalDrives().addAll(asLogicalDriveContexts(source.getUsedLogicalDrives()));
        links.getTargets().addAll(asRemoteTargetContexts(source.getRemoteTargets()));
    }
}
