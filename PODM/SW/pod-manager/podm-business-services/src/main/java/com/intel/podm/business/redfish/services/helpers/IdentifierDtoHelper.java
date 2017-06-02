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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.dto.redfish.attributes.IdentifierDto;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.types.redfish.DurableIdentifier;

import java.util.List;

import static java.util.stream.Collectors.toList;

public final class IdentifierDtoHelper {
    private IdentifierDtoHelper() {
    }

    public List<DurableIdentifier> translateIdentifiersToDto(List<Identifier> identifiers) {
        return identifiers.stream().map(
            identifier -> IdentifierDto.newBuilder()
                .durableName(identifier.getDurableName())
                .durableNameFormat(identifier.getDurableNameFormat())
                .build())
            .collect(toList());
    }
}
