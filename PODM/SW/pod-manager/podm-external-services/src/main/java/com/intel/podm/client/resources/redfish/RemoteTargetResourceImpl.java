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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.IscsiAddressObject;
import com.intel.podm.client.api.resources.redfish.RemoteTargetResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static java.util.stream.Collectors.toList;

@OdataTypes({
        "#RemoteTarget" + OdataTypes.VERSION_PATTERN + "RemoteTarget"
})
public class RemoteTargetResourceImpl extends ExternalServiceResourceImpl implements RemoteTargetResource {
    @JsonProperty("Status")
    private Status status;

    @JsonProperty ("Type")
    private String type;

    @JsonProperty("Addresses")
    private List<AddressWrapper> addresses = new ArrayList<>();

    @JsonProperty("Initiator")
    private List<Initiator> initiator = new ArrayList<>();

    @Override
    public String getType() {
        return type;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public List<IscsiAddressObject> getAddresses() {
        return addresses.stream().map((wrap) -> wrap.iscsi).collect(toList());
    }

    @Override
    @LinkName("logicalDrives")
    public Iterable<ResourceSupplier> getLogicalDrives() throws ExternalServiceApiReaderException {
        List<ODataId> logicalDrives = addresses
                .stream()
                .map(address -> address.iscsi.getTargetLuns())
                .flatMap(Collection::stream)
                .map(targetLun -> ((TargetLunObjectImpl) targetLun).getLogicalDrive())
                .collect(toList());

        return toSuppliers(logicalDrives);
    }

    @Override
    public String getIscsiInitiatorIqn() {
        Initiator initiator = this.initiator.stream().findFirst().orElse(null);

        if (initiator != null) {
            return initiator.iscsi.initiatorIqn;
        }
        return null;
    }

    public static final class AddressWrapper {
        @JsonProperty("iSCSI")
        private IscsiAddressObjectImpl iscsi;
    }

    private static class Initiator {
        @JsonProperty("iSCSI")
        private Iscsi iscsi = new Iscsi();

        private static class Iscsi {
            @JsonProperty("InitiatorIQN")
            private String initiatorIqn;
        }
    }
}
