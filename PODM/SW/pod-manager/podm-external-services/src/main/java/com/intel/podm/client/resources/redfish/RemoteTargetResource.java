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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static java.util.stream.Collectors.toList;

@OdataTypes({
    "#RemoteTarget" + VERSION_PATTERN + "RemoteTarget"
})
public class RemoteTargetResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Status")
    private Status status;

    @JsonProperty("Type")
    private String type;

    @JsonProperty("Addresses")
    private List<AddressWrapper> addresses = new ArrayList<>();

    @JsonProperty("Initiator")
    private List<InitiatorObject> initiator = new ArrayList<>();

    public String getType() {
        return type;
    }

    public Status getStatus() {
        return status;
    }

    public List<IscsiAddressObject> getAddresses() {
        return addresses.stream().map((wrap) -> wrap.iscsi).collect(toList());
    }

    @LinkName("logicalDrives")
    public Iterable<ResourceSupplier> getLogicalDrives() throws WebClientRequestException {
        List<ODataId> logicalDrives = addresses
            .stream()
            .map(address -> address.iscsi.getTargetLuns())
            .flatMap(Collection::stream)
            .map(TargetLunObject::getLogicalDrive)
            .collect(toList());

        return toSuppliers(logicalDrives);
    }

    public String getIscsiInitiatorIqn() {
        InitiatorObject initiator = this.initiator.stream().findFirst().orElse(null);

        if (initiator != null) {
            return initiator.iscsi.initiatorIqn;
        }
        return null;
    }

    public static final class AddressWrapper {
        @JsonProperty("iSCSI")
        private IscsiAddressObject iscsi;
    }

    public static class InitiatorObject {
        @JsonProperty("iSCSI")
        private IscsiObject iscsi = new IscsiObject();

        public static class IscsiObject {
            @JsonProperty("InitiatorIQN")
            private String initiatorIqn;
        }
    }
}
