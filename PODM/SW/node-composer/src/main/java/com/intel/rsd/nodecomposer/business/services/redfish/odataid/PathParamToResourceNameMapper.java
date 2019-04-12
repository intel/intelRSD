/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.services.redfish.odataid;

import java.util.HashMap;
import java.util.Map;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.PathParamConstants.COMPOSED_NODE_ID;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.Collections.unmodifiableMap;

public final class PathParamToResourceNameMapper {
    static final Map<String, String> MAPPING;

    static {
        HashMap<String, String> map = new HashMap<>();
        map.put(COMPOSED_NODE_ID, COMPOSED_NODES_RESOURCE_NAME);
        MAPPING = unmodifiableMap(map);
    }

    private PathParamToResourceNameMapper() {
    }

    public static String getResourceName(String pathParam) {
        requiresNonNull(pathParam, "pathParam");

        return MAPPING.get(pathParam);
    }
}
