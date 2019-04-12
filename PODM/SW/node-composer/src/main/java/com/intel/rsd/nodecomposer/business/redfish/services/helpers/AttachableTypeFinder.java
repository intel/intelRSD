/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.helpers;

import com.intel.rsd.nodecomposer.business.RequestValidationException;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.AttachableType;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;

import static com.intel.rsd.nodecomposer.business.Violations.createWithViolations;
import static com.intel.rsd.nodecomposer.types.AttachableType.DRIVE;
import static com.intel.rsd.nodecomposer.types.AttachableType.ENDPOINT;
import static com.intel.rsd.nodecomposer.types.AttachableType.PROCESSOR;
import static com.intel.rsd.nodecomposer.types.AttachableType.VOLUME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.DRIVES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.PROCESSOR_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static java.lang.String.format;

public final class AttachableTypeFinder {
    private static final Map<String, AttachableType> RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING = new HashMap<>();

    static {
        RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING.put(ENDPOINTS_RESOURCE_NAME, ENDPOINT);
        RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING.put(VOLUMES_RESOURCE_NAME, VOLUME);
        RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING.put(DRIVES_RESOURCE_NAME, DRIVE);
        RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING.put(PROCESSOR_RESOURCE_NAME, PROCESSOR);
    }

    private AttachableTypeFinder() {
    }

    public static AttachableType getAttachableTypeByODataId(ODataId oDataId) throws RequestValidationException {
        AttachableType attachableType = tryFindAttachableType(oDataId);

        if (attachableType == null) {
            throw new RequestValidationException(createWithViolations(format("Attach/Detach action is not supported for  %s.", oDataId)));
        }

        return attachableType;
    }

    private static AttachableType tryFindAttachableType(ODataId oDataId) {
        Function<String, AttachableType> retrieveType = oDataIdString -> RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING.keySet()
            .stream()
            .filter(oDataIdString::contains)
            .map(RESOURCE_NAME_TO_ATTACHABLE_TYPE_MAPPING::get)
            .findFirst()
            .orElse(null);

        return retrieveType.apply(oDataId.toString());
    }
}
