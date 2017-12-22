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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.List;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name",
    "description", "status", "type", "addresses", "Initiator", "oem", "links"
})
public final class RemoteTargetDto extends RedfishDto {
    private Status status;
    private String type;
    @JsonProperty("Addresses")
    private List<AddressWrapper> addresses = new ArrayList<>();
    @JsonProperty("Initiator")
    private List<InitiatorWrapper> initiators = new ArrayList<>();
    private Links links = new Links();

    public RemoteTargetDto() {
        super("#RemoteTarget.v1_1_0.RemoteTarget");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public List<AddressWrapper> getAddresses() {
        return addresses;
    }

    public void setAddresses(List<AddressWrapper> addresses) {
        this.addresses = addresses;
    }

    public List<InitiatorWrapper> getInitiators() {
        return initiators;
    }

    public void setInitiators(List<InitiatorWrapper> initiators) {
        this.initiators = initiators;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    public static class AddressWrapper {
        @JsonProperty("iSCSI")
        private final IscsiAddressDto iscsiAddressDto = new IscsiAddressDto();

        public IscsiAddressDto getIscsiAddressDto() {
            return iscsiAddressDto;
        }

        @JsonPropertyOrder({"@odata.type", "TargetLUN", "TargetIQN", "TargetPortalIP", "TargetPortalPort"})
        @JsonInclude(NON_NULL)
        @SuppressWarnings({"checkstyle:VisibilityModifier"})
        public static class IscsiAddressDto {
            @JsonProperty("@odata.type")
            private final String oDataType = "#RemoteTarget.v1_1_0.ISCSIAddress";
            @JsonProperty("TargetLUN")
            private List<TargetLunDto> targetLuns = new ArrayList<>();
            @JsonProperty("TargetIQN")
            private String targetIqn;
            @JsonProperty("TargetPortalIP")
            private String targetPortalIp;
            @JsonProperty("TargetPortalPort")
            private Integer targetPortalPort;
            @JsonProperty("CHAP")
            private Chap chap = new Chap();

            public String getoDataType() {
                return oDataType;
            }

            public List<TargetLunDto> getTargetLuns() {
                return targetLuns;
            }

            public void setTargetLuns(List<TargetLunDto> targetLuns) {
                this.targetLuns = targetLuns;
            }

            public String getTargetIqn() {
                return targetIqn;
            }

            public void setTargetIqn(String targetIqn) {
                this.targetIqn = targetIqn;
            }

            public String getTargetPortalIp() {
                return targetPortalIp;
            }

            public void setTargetPortalIp(String targetPortalIp) {
                this.targetPortalIp = targetPortalIp;
            }

            public Integer getTargetPortalPort() {
                return targetPortalPort;
            }

            public void setTargetPortalPort(Integer targetPortalPort) {
                this.targetPortalPort = targetPortalPort;
            }

            public Chap getChap() {
                return chap;
            }

            public void setChap(Chap chap) {
                this.chap = chap;
            }

            @JsonPropertyOrder({"LUN", "LogicalDrive"})
            public static final class TargetLunDto {
                @JsonProperty("LUN")
                private Integer lun;
                @JsonProperty("LogicalDrive")
                private Context logicalDrive;

                public Integer getLun() {
                    return lun;
                }

                public void setLun(Integer lun) {
                    this.lun = lun;
                }

                public Context getLogicalDrive() {
                    return logicalDrive;
                }

                public void setLogicalDrive(Context logicalDrive) {
                    this.logicalDrive = logicalDrive;
                }
            }

            @JsonPropertyOrder({"oDataType", "Type", "Username", "Secret", "MutualUsername", "MutualSecret"})
            public static final class Chap {
                @JsonProperty("@odata.type")
                private String oDataType = "#RemoteTarget.v1_1_0.Chap";
                @JsonProperty("Type")
                private ChapType type;
                @JsonProperty("Username")
                private String username;
                @JsonProperty("Secret")
                private String secret;
                @JsonProperty("MutualUsername")
                private String mutualUsername;
                @JsonProperty("MutualSecret")
                private String mutualSecret;

                public String getoDataType() {
                    return oDataType;
                }

                public ChapType getType() {
                    return type;
                }

                public void setType(ChapType type) {
                    this.type = type;
                }

                public String getUsername() {
                    return username;
                }

                public void setUsername(String username) {
                    this.username = username;
                }

                public String getSecret() {
                    return secret;
                }

                public void setSecret(String secret) {
                    this.secret = secret;
                }

                public String getMutualUsername() {
                    return mutualUsername;
                }

                public void setMutualUsername(String mutualUsername) {
                    this.mutualUsername = mutualUsername;
                }

                public String getMutualSecret() {
                    return mutualSecret;
                }

                public void setMutualSecret(String mutualSecret) {
                    this.mutualSecret = mutualSecret;
                }
            }
        }
    }

    public static class InitiatorWrapper {
        @JsonProperty("iSCSI")
        private final IscsiInitiatorDto iscsiInitiatorDto = new IscsiInitiatorDto();

        public IscsiInitiatorDto getIscsiInitiatorDto() {
            return iscsiInitiatorDto;
        }

        @JsonPropertyOrder({"InitiatorIQN"})
        @SuppressWarnings({"checkstyle:VisibilityModifier"})
        public static class IscsiInitiatorDto {
            @JsonProperty("InitiatorIQN")
            private String initiatorIqn;

            public String getInitiatorIqn() {
                return initiatorIqn;
            }

            public void setInitiatorIqn(String initiatorIqn) {
                this.initiatorIqn = initiatorIqn;
            }

        }
    }

    public class Links extends RedfishLinksDto {

    }
}
