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

package com.intel.podm.common.types.actions;

import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.RemoteTargetUpdateDefinition.Address.IScsiAddress;
import com.intel.podm.common.types.actions.RemoteTargetUpdateDefinition.Address.IScsiAddress.Chap;
import com.intel.podm.common.types.actions.RemoteTargetUpdateDefinition.Initiator.IScsiInitiator;
import com.intel.podm.common.types.redfish.RedfishRemoteTarget;

import java.util.List;

import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class RemoteTargetUpdateDefinition {
    private List<Address> addresses;
    private List<Initiator> initiator;

    public RemoteTargetUpdateDefinition(RedfishRemoteTarget representation) {
        addAddressesToDefinition(representation);
        addInitiatorsToDefinition(representation);
    }

    private void addAddressesToDefinition(RedfishRemoteTarget representation) {
        List<RedfishRemoteTarget.Address> addresses = representation.getAddresses();

        if (addresses == null) {
            return;
        }

        this.addresses = addresses.stream()
            .map(this::mapAddress)
            .collect(toList());
    }

    private void addInitiatorsToDefinition(RedfishRemoteTarget representation) {
        List<RedfishRemoteTarget.Initiator> initiators = representation.getInitiator();

        if (initiators == null) {
            return;
        }

        this.initiator = initiators.stream()
            .map(this::mapInitiators)
            .collect(toList());
    }

    private Initiator mapInitiators(RedfishRemoteTarget.Initiator initiator) {
        Initiator initiatorDef = new Initiator();
        ofNullable(initiator.getIScsiInitators()).map(this::mapIScsiInitiator).ifPresent(initiatorDef::setIScsiInitiator);

        return initiatorDef;
    }

    private IScsiInitiator mapIScsiInitiator(RedfishRemoteTarget.Initiator.IScsiInitiator iScsiInitiator) {
        IScsiInitiator iScsiInitiatorDef = new IScsiInitiator();
        ofNullable(iScsiInitiator.getInitiatorIqn()).ifPresent(iScsiInitiatorDef::setInitiatorIqn);
        return iScsiInitiatorDef;
    }


    private Address mapAddress(RedfishRemoteTarget.Address address) {
        Address addressDef = new Address();
        ofNullable(address.getIScsiAddresses()).map(this::mapIScsiAddress).ifPresent(addressDef::setIScsiAddress);
        return addressDef;
    }

    private IScsiAddress mapIScsiAddress(RedfishRemoteTarget.Address.IScsiAddress iScsiAddress) {
        IScsiAddress iScsiAddressDef = new IScsiAddress();
        iScsiAddress.getChap().ifAssigned(chap -> iScsiAddressDef.setChap(mapChap(chap)));
        return iScsiAddressDef;
    }

    private Ref<Chap> mapChap(RedfishRemoteTarget.Address.IScsiAddress.Chap chap) {
        Chap chapDef = null;
        if (chap != null) {
            chapDef = new Chap();
            chapDef.setType(chap.getType());
            chapDef.setUsername(chap.getUsername());
            chapDef.setSecret(chap.getSecret());
            chapDef.setMutualUsername(chap.getMutualUsername());
            chapDef.setMutualSecret(chap.getMutualSecret());
        }
        return Ref.of(chapDef);
    }

    public List<Address> getAddresses() {
        return addresses;
    }

    public List<Initiator> getInitiator() {
        return initiator;
    }

    public static class Address {

        private IScsiAddress iScsiAddress;

        public IScsiAddress getIScsiAddress() {
            return iScsiAddress;
        }

        public void setIScsiAddress(IScsiAddress iScsiAddress) {
            this.iScsiAddress = iScsiAddress;
        }

        public static class IScsiAddress {

            private Ref<Chap> chap;

            public Ref<Chap> getChap() {
                return chap;
            }

            public void setChap(Ref<Chap> chap) {
                this.chap = chap;
            }

            public static class Chap {
                private Ref<ChapType> type;
                private Ref<String> username;
                private Ref<String> secret;
                private Ref<String> mutualUsername;
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
        private IScsiInitiator iScsiInitiator;

        public IScsiInitiator getIScsiInitiator() {
            return iScsiInitiator;
        }

        public void setIScsiInitiator(IScsiInitiator iScsiInitiator) {
            this.iScsiInitiator = iScsiInitiator;
        }

        public static class IScsiInitiator {
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
