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

import java.util.List;

import static java.util.Collections.singletonList;

public final class PathParamConstants {
    public static final String COMPOSED_NODE_ID = "composedNodeId";

    private static final List<String> TOPOLOGICALLY_ORDERED_PARAMETERS;

    static {
        TOPOLOGICALLY_ORDERED_PARAMETERS = singletonList(COMPOSED_NODE_ID);
    }

    private PathParamConstants() {
    }

    public static List<String> getPathParameterNames() {
        return TOPOLOGICALLY_ORDERED_PARAMETERS;
    }
}
