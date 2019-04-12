/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import java.net.URI;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

public final class ODataContextProvider {
    private static final String METADATA_PREFIX = "/redfish/v1/$metadata";

    private ODataContextProvider() {
    }

    public static URI getContextFromODataType(String oDataType) {
        if (oDataType.isEmpty()) {
            throw new IllegalStateException("No mapping for empty oDataType");
        }

        String resourceType = oDataType.replaceAll(VERSION_PATTERN, ".");

        String context = METADATA_PREFIX + resourceType;
        return URI.create(context);
    }
}
