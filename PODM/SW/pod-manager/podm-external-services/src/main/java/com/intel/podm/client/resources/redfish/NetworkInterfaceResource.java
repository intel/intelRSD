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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OdataTypeVersions;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

@OdataTypes({
    "#NetworkInterface" + OdataTypeVersions.VERSION_PATTERN + "NetworkInterface"
})
public class NetworkInterfaceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @JsonProperty("NetworkDeviceFunctions")
    private ODataId networkDeviceFunctions;

    public Ref<Status> getStatus() {
        return status;
    }

    @LinkName("networkDeviceFunctions")
    public Iterable<ResourceSupplier> getNetworkDeviceFunctions() throws WebClientRequestException {
        return processMembersListResource(networkDeviceFunctions);
    }
}
