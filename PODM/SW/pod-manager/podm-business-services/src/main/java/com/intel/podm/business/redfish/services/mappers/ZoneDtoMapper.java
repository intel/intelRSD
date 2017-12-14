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

import com.intel.podm.business.dto.ZoneDto;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.redfish.Contexts;

import javax.enterprise.context.Dependent;

import static java.util.stream.Collectors.toSet;

@Dependent
class ZoneDtoMapper extends DtoMapper<Zone, ZoneDto> {
    ZoneDtoMapper() {
        super(Zone.class, ZoneDto.class);
        this.ignoredProperties("links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(Zone source, ZoneDto target) {
        super.performNotAutomatedMapping(source, target);

        target.getLinks().getEndpoints().addAll(
            source.getEndpoints().stream()
                .map(Contexts::toContext)
                .collect(toSet())
        );

        target.getLinks().getInvolvedSwitches().addAll(
            source.getSwitches().stream()
                .map(Contexts::toContext)
                .collect(toSet())
        );
    }
}
