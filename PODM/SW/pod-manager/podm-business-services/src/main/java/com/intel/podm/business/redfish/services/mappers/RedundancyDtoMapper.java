/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.dto.RedundancyDto;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.util.stream.Collectors.toSet;

@Dependent
class RedundancyDtoMapper extends BrilliantMapper<Redundancy, RedundancyDto> {
    RedundancyDtoMapper() {
        super(Redundancy.class, RedundancyDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(Redundancy source, RedundancyDto target) {
        super.performNotAutomatedMapping(source, target);
        target.setoDataId(asOdataId(toContext(source)));
        target.setRedundancySet(source.getRedundancyMembers().stream()
            .map(Contexts::toContext)
            .collect(toSet()));
    }
}
