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

package com.intel.rsd.nodecomposer.rest.redfish.resources.odataid;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.Id;

import java.util.LinkedHashMap;
import java.util.Map.Entry;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromString;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.PathParamToResourceNameMapper.getResourceName;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

public final class ODataIdBuilder {
    private final LinkedHashMap<String, Id> params = new LinkedHashMap<>();

    public void add(String paramName, Id value) {
        requiresNonNull(value, "value");
        params.put(paramName, value);
    }

    public ODataId build() throws ODataIdBuilderException {
        StringBuilder path = new StringBuilder();

        for (Entry<String, Id> entry : params.entrySet()) {
            String paramName = entry.getKey();
            Id id = entry.getValue();

            String resourceName = getResourceName(paramName);

            path.append("/").append(resourceName).append("/").append(id);
        }

        if (path.length() == 0) {
            throw new ODataIdBuilderException("No parameters has been added");
        }

        return oDataIdFromString("/redfish/v1" + path.toString());
    }
}
