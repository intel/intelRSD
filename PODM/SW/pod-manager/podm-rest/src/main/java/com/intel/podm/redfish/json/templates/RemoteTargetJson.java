/*
 * Copyright (c) 2015 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.IscsiAddressJson;
import com.intel.podm.redfish.json.templates.attributes.IscsiInitiatorJson;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name",
        "description", "status", "type", "addresses", "initiator", "oem", "links"
})
public class RemoteTargetJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public Status status;
    public String type;
    public List<AddressWrapper> addresses = new ArrayList<>();
    public List<InitiatorWrapper> initiator = new ArrayList<>();
    public Object oem = new Object();
    public Links links = new Links();

    public RemoteTargetJson() {
        super("#RemoteTarget.1.0.0.RemoteTarget");
    }

    public static class AddressWrapper {
        @JsonProperty("iSCSI")
        public IscsiAddressJson iscsiAddressJson = new IscsiAddressJson();
    }

    public static class InitiatorWrapper {
        @JsonProperty("iSCSI")
        public IscsiInitiatorJson iscsiInitiatorJson = new IscsiInitiatorJson();
    }

    public static class Links {
        public Object oem = new Object();
    }
}
