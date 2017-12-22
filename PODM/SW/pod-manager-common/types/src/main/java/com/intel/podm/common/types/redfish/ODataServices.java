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

package com.intel.podm.common.types.redfish;

import java.util.List;

import static com.intel.podm.common.types.redfish.ResourceNames.CHASSIS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPUTER_SYSTEM_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SERVICE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.FABRIC_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.TELEMETRY_SERVICE_RESOURCE_NAME;
import static java.util.Arrays.asList;
import static java.util.Collections.unmodifiableList;

public final class ODataServices {
    public static final List<String> ODATA_ROOT_SERVICES;

    static {
        ODATA_ROOT_SERVICES = unmodifiableList(asList(
            COMPUTER_SYSTEM_RESOURCE_NAME,
            CHASSIS_RESOURCE_NAME,
            MANAGERS_RESOURCE_NAME,
            COMPOSED_NODES_RESOURCE_NAME,
            STORAGE_SERVICES_RESOURCE_NAME,
            ETHERNET_SWITCHES_RESOURCE_NAME,
            FABRIC_RESOURCE_NAME,
            EVENT_SERVICE_RESOURCE_NAME,
            TELEMETRY_SERVICE_RESOURCE_NAME
        ));
    }

    private ODataServices() {
    }
}
