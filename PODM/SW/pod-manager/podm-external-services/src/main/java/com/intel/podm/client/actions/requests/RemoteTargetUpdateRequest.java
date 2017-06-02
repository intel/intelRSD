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

package com.intel.podm.client.actions.requests;


import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.RemoteTargetUpdateDefinition;

import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

@JsonInclude(NON_NULL)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class RemoteTargetUpdateRequest {

    @JsonProperty("Addresses")
    private List<Address> addresses;

    @JsonProperty("Initiator")
    private List<Initiator> initiators;

    public RemoteTargetUpdateRequest() {
    }

    public RemoteTargetUpdateRequest(RemoteTargetUpdateDefinition definition) {
        addAddresses(definition);
        addInitiators(definition);
    }

    private void addInitiators(RemoteTargetUpdateDefinition representation) {
        List<RemoteTargetUpdateDefinition.Initiator> initiators = representation.getInitiator();

        if (initiators == null) {
            return;
        }

        this.initiators = initiators.stream()
            .map(this::mapInitiators)
            .collect(toList());
    }

    private RemoteTargetUpdateRequest.Initiator mapInitiators(RemoteTargetUpdateDefinition.Initiator initiator) {
        RemoteTargetUpdateRequest.Initiator initiatorDef = new RemoteTargetUpdateRequest.Initiator();
        ofNullable(initiator.getIScsiInitiator()).map(this::mapIScsiInitiator).ifPresent(initiatorDef::setIScsiInitiator);

        return initiatorDef;
    }

    private RemoteTargetUpdateRequest.Initiator.IScsiInitiator mapIScsiInitiator(RemoteTargetUpdateDefinition.Initiator.IScsiInitiator iScsiInitiator) {
        RemoteTargetUpdateRequest.Initiator.IScsiInitiator iScsiInitiatorDef = new RemoteTargetUpdateRequest.Initiator.IScsiInitiator();
        iScsiInitiator.getInitiatorIqn().ifAssigned(s -> iScsiInitiatorDef.setInitiatorIqn(Ref.of(s)));
        return iScsiInitiatorDef;
    }

    private void addAddresses(RemoteTargetUpdateDefinition representation) {
        List<RemoteTargetUpdateDefinition.Address> addresses = representation.getAddresses();

        if (addresses == null) {
            return;
        }

        this.addresses = addresses.stream()
            .map(this::mapAddress)
            .collect(toList());
    }

    private RemoteTargetUpdateRequest.Address mapAddress(RemoteTargetUpdateDefinition.Address address) {
        RemoteTargetUpdateRequest.Address addressDef = new RemoteTargetUpdateRequest.Address();
        ofNullable(address.getIScsiAddress()).map(this::mapIScsiAddress).ifPresent(addressDef::setIScsiAddress);
        return addressDef;
    }

    private RemoteTargetUpdateRequest.Address.IScsiAddress mapIScsiAddress(RemoteTargetUpdateDefinition.Address.IScsiAddress iScsiAddress) {
        RemoteTargetUpdateRequest.Address.IScsiAddress iScsiAddressDef = new RemoteTargetUpdateRequest.Address.IScsiAddress();
        iScsiAddress.getChap().ifAssigned(chap -> iScsiAddressDef.setChap(mapChap(chap)));
        return iScsiAddressDef;
    }

    private Ref<RemoteTargetUpdateRequest.Address.IScsiAddress.Chap> mapChap(RemoteTargetUpdateDefinition.Address.IScsiAddress.Chap chap) {
        Ref<RemoteTargetUpdateRequest.Address.IScsiAddress.Chap> refChap = Ref.of(null);
        if (chap != null) {
            RemoteTargetUpdateRequest.Address.IScsiAddress.Chap chapDef = new RemoteTargetUpdateRequest.Address.IScsiAddress.Chap();
            chap.getType().ifAssigned(chapType -> chapDef.setType(Ref.of(chapType)));
            chap.getUsername().ifAssigned(s -> chapDef.setUsername(Ref.of(s)));
            chap.getSecret().ifAssigned(s -> chapDef.setSecret(Ref.of(s)));
            chap.getMutualUsername().ifAssigned(s -> chapDef.setMutualUsername(Ref.of(s)));
            chap.getMutualSecret().ifAssigned(s -> chapDef.setMutualSecret(Ref.of(s)));
            refChap = Ref.of(chapDef);
        }
        return refChap;
    }

    public List<Address> getAddresses() {
        return addresses;
    }

    public List<Initiator> getInitiators() {
        return initiators;
    }

    public static class Address {

        @JsonProperty("iSCSI")
        private IScsiAddress iScsiAddress;

        public void setIScsiAddress(IScsiAddress iScsiAddress) {
            this.iScsiAddress = iScsiAddress;
        }

        public static class IScsiAddress {

            @JsonProperty("CHAP")
            private Ref<Chap> chap;

            public Ref<Chap> getChap() {
                return chap;
            }

            public void setChap(Ref<Chap> chap) {
                this.chap = chap;
            }

            @JsonInclude(NON_DEFAULT)
            public static class Chap {
                @JsonProperty("Type")
                private Ref<ChapType> type;
                @JsonProperty("Username")
                private Ref<String> username;
                @JsonProperty("Secret")
                private Ref<String> secret;
                @JsonProperty("MutualUsername")
                private Ref<String> mutualUsername;
                @JsonProperty("MutualSecret")
                private Ref<String> mutualSecret;

                public Ref<ChapType> getType() {
                    return type;
                }

                public void setType(Ref<ChapType> type) {
                    this.type = type;
                }

                public Ref<String> getUsername() {
                    return username;
                }

                public void setUsername(Ref<String> username) {
                    this.username = username;
                }

                public Ref<String> getSecret() {
                    return secret;
                }

                public void setSecret(Ref<String> secret) {
                    this.secret = secret;
                }

                public Ref<String> getMutualUsername() {
                    return mutualUsername;
                }

                public void setMutualUsername(Ref<String> mutualUsername) {
                    this.mutualUsername = mutualUsername;
                }

                public Ref<String> getMutualSecret() {
                    return mutualSecret;
                }

                public void setMutualSecret(Ref<String> mutualSecret) {
                    this.mutualSecret = mutualSecret;
                }
            }
        }
    }

    public static class Initiator {
        @JsonProperty("iSCSI")
        private IScsiInitiator iScsiInitiator;

        public void setIScsiInitiator(IScsiInitiator iScsiInitiator) {
            this.iScsiInitiator = iScsiInitiator;
        }

        @JsonInclude(NON_DEFAULT)
        public static class IScsiInitiator {
            @JsonProperty("InitiatorIQN")
            private Ref<String> initiatorIqn;

            public Ref<String> getInitiatorIqn() {
                return initiatorIqn;
            }

            public void setInitiatorIqn(Ref<String> initiatorIqn) {
                this.initiatorIqn = initiatorIqn;
            }
        }
    }
}
