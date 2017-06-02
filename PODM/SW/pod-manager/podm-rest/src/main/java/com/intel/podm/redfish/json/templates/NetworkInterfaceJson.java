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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Status;
import com.intel.podm.business.services.redfish.odataid.ODataId;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "status", "links", "networkDeviceFunctions", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class NetworkInterfaceJson extends BaseResourceJson {
    public Status status;
    public Links links = new Links();
    public ODataId networkDeviceFunctions;

    public NetworkInterfaceJson() {
        super("#NetworkInterface.v1_0_0.NetworkInterface");
    }

    public final class Links extends RedfishLinksJson {
    }
}
