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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishRemoteTarget;

import java.util.List;

import static com.intel.podm.common.types.Ref.unassigned;

public class RemoteTargetPartialRepresentation implements RedfishRemoteTarget {

    @JsonProperty("Addresses")
    private List<Address> addresses;

    @JsonProperty("Initiator")
    private List<Initiator> initiator;

    public List<RedfishRemoteTarget.Address> getAddresses() {
        return (List) addresses;
    }

    @Override
    public List<RedfishRemoteTarget.Initiator> getInitiator() {
        return (List) initiator;
    }

    public static class Address implements RedfishRemoteTarget.Address {

        @JsonProperty("iSCSI")
        private IScsiAddress iScsiAddress;

        @Override
        public RedfishRemoteTarget.Address.IScsiAddress getIScsiAddresses() {
            return iScsiAddress;
        }

        public static class IScsiAddress implements RedfishRemoteTarget.Address.IScsiAddress {

            @JsonProperty("CHAP")
            private Ref<Chap> chap = unassigned();

            @Override
            public Ref<RedfishRemoteTarget.Address.IScsiAddress.Chap> getChap() {
                return (Ref) chap;
            }

            public static class Chap implements RedfishRemoteTarget.Address.IScsiAddress.Chap {
                @JsonProperty("Type")
                private Ref<ChapType> type = unassigned();
                @JsonProperty("Username")
                private Ref<String> username = unassigned();
                @JsonProperty("Secret")
                private Ref<String> secret = unassigned();
                @JsonProperty("MutualUsername")
                private Ref<String> mutualUsername = unassigned();
                @JsonProperty("MutualSecret")
                private Ref<String> mutualSecret = unassigned();

                public Ref<ChapType> getType() {
                    return type;
                }

                public Ref<String> getUsername() {
                    return username;
                }

                public Ref<String> getSecret() {
                    return secret;
                }

                public Ref<String> getMutualUsername() {
                    return mutualUsername;
                }

                public Ref<String> getMutualSecret() {
                    return mutualSecret;
                }
            }
        }
    }

    public static class Initiator implements RedfishRemoteTarget.Initiator {

        @JsonProperty("iSCSI")
        private IScsiInitiator iScsiInitiator;

        @Override
        public RedfishRemoteTarget.Initiator.IScsiInitiator getIScsiInitators() {
            return iScsiInitiator;
        }

        public static class IScsiInitiator implements RedfishRemoteTarget.Initiator.IScsiInitiator {
            @JsonProperty("InitiatorIQN")
            private Ref<String> inititatorIqn = unassigned();

            @Override
            public Ref<String> getInitiatorIqn() {
                return inititatorIqn;
            }
        }
    }
}
