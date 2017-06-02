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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.NetworkInterfaceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.annotations.AsUnassigned;

import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;

@OdataTypes({
    "#NetworkInterface" + OdataTypes.VERSION_PATTERN + "NetworkInterface"
})
public class NetworkInterfaceResourceImpl extends ExternalServiceResourceImpl implements NetworkInterfaceResource {

    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();

    @JsonProperty("NetworkDeviceFunctions")
    private ODataId networkDeviceFunctions;

    @Override
    public Ref<Status> getStatus() {
        return status;
    }

    @Override
    @LinkName("networkDeviceFunctions")
    public Iterable<ResourceSupplier> getNetworkDeviceFunctions() throws ExternalServiceApiReaderException {
        return processMembersListResource(networkDeviceFunctions);
    }
}
