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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.RedundancyDto;
import com.intel.podm.redfish.json.templates.attributes.RedundancyJson;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Collectors;

import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;

public final class RedundancyDtoJsonMapper {

    private RedundancyDtoJsonMapper() {
    }

    public static Function<RedundancyDto, RedundancyJson> toRedundancy() {
        return dto -> {
            RedundancyJson json = new RedundancyJson();
            json.oDataId = asOdataId(dto.getContext());
            json.memberId = dto.getMemberId();
            json.name = dto.getName();
            json.status = dto.getStatus();
            json.maxNumSupported = dto.getMaxNumSupported();
            json.minNumNeeded = dto.getMinNumNeeded();
            json.mode = dto.getMode();
            json.redundancyEnabled = dto.getRedundancyEnabled();
            json.redundancySet.addAll(asOdataIdSet(dto.getRedundancySet()));
            return json;
        };
    }

    public static Collection<RedundancyJson> getRedundanciesCollection(List<RedundancyDto> redundancyDtos) {
        return redundancyDtos.stream()
            .map(redundancyDto -> toRedundancy(redundancyDto, String.valueOf(redundancyDtos.indexOf(redundancyDto))))
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private static RedundancyJson toRedundancy(RedundancyDto dto, String jsonPointerId) {
        RedundancyJson json = new RedundancyJson();
        json.oDataId = asJsonPointerOdataId(dto.getContext(), jsonPointerId);
        json.memberId = dto.getMemberId();
        json.name = dto.getName();
        json.status = dto.getStatus();
        json.maxNumSupported = dto.getMaxNumSupported();
        json.minNumNeeded = dto.getMinNumNeeded();
        json.mode = dto.getMode();
        json.redundancyEnabled = dto.getRedundancyEnabled();
        json.redundancySet.addAll(asJsonPointerOdataIdSet(dto.getRedundancySet(), jsonPointerId));
        return json;
    }
}
