/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.actions.requests;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.api.actions.RemoteTargetResourceCreationRequest;

import java.net.URI;
import java.util.List;

import static java.util.Collections.singletonList;

@JsonPropertyOrder({"Name", "Type", "Addresses", "Initiator"})
public class RemoteTargetResourceCreationRequestImpl implements RemoteTargetResourceCreationRequest {
    @JsonProperty("Name")
    private final String name = "Remote Target";

    @JsonProperty("Type")
    private final String type = "Network Storage";

    @JsonProperty("Addresses")
    private List<WrappedIscsiAddress> iScsiAddressList;

    @JsonProperty("Initiator")
    private List<IscsiInitiatorIqn> iscsiInitiatorIqnList;

    @Override
    public void setLunInfo(URI logicalDrive, String targetIqn) {
        iScsiAddressList = singletonList(new WrappedIscsiAddress(logicalDrive, targetIqn));
    }

    @Override
    public void setInitiatorIqn(String initiatorIqn) {
        iscsiInitiatorIqnList = singletonList(new IscsiInitiatorIqn(initiatorIqn));
    }
}
