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

import com.intel.podm.business.dto.EthernetSwitchDto;
import com.intel.podm.business.entities.redfish.EthernetSwitch;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
class EthernetSwitchDtoMapper extends DtoMapper<EthernetSwitch, EthernetSwitchDto> {
    EthernetSwitchDtoMapper() {
        super(EthernetSwitch.class, EthernetSwitchDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitch source, EthernetSwitchDto target) {
        super.performNotAutomatedMapping(source, target);
        target.setLinks(target.createLinks(toContext(source.getChassis()), asManagerContexts(source.getManagers())));
    }
}
