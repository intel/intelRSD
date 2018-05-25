/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "status", "networkDeviceFunctions", "links", "oem"})
public final class NetworkInterfaceDto extends RedfishDto {
    private final Links links = new Links();
    private Status status;
    private SingletonContext networkDeviceFunctions;

    public NetworkInterfaceDto() {
        super("#NetworkInterface.v1_0_0.NetworkInterface");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Links getLinks() {
        return links;
    }

    public SingletonContext getNetworkDeviceFunctions() {
        return networkDeviceFunctions;
    }

    public void setNetworkDeviceFunctions(SingletonContext networkDeviceFunctions) {
        this.networkDeviceFunctions = networkDeviceFunctions;
    }

    public final class Links extends RedfishLinksDto {
    }
}
