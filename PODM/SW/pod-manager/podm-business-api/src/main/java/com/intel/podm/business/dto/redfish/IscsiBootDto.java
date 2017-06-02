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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class IscsiBootDto {
    private final IpAddressType ipAddressType;
    private final String initiatorIpAddress;
    private final String initiatorName;
    private final String initiatorDefaultGateway;
    private final String initiatorNetmask;
    private final Boolean targetInfoViaDhcp;
    private final String primaryTargetName;
    private final String primaryTargetIpAddress;
    private final Integer primaryTargetTcpPort;
    private final Integer primaryLun;
    private final Boolean primaryVlanEnable;
    private final Integer primaryVlanId;
    private final String primaryDns;
    private final String secondaryTargetName;
    private final String secondaryTargetIpAddress;
    private final Integer secondaryTargetTcpPort;
    private final Integer secondaryLun;
    private final Boolean secondaryVlanEnable;
    private final Integer secondaryVlanId;
    private final String secondaryDns;
    private final Boolean ipMaskDnsViaDhcp;
    private final Boolean routerAdvertisementEnabled;
    private final AuthenticationMethod authenticationMethod;
    private final String chapUsername;
    private final String chapSecret;
    private final String mutualChapUsername;
    private final String mutualChapSecret;

    private IscsiBootDto(Builder builder) {
        ipAddressType = builder.ipAddressType;
        initiatorIpAddress = builder.initiatorIpAddress;
        initiatorName = builder.initiatorName;
        initiatorDefaultGateway = builder.initiatorDefaultGateway;
        initiatorNetmask = builder.initiatorNetmask;
        targetInfoViaDhcp = builder.targetInfoViaDhcp;
        primaryTargetName = builder.primaryTargetName;
        primaryTargetIpAddress = builder.primaryTargetIpAddress;
        primaryTargetTcpPort = builder.primaryTargetTcpPort;
        primaryLun = builder.primaryLun;
        primaryVlanEnable = builder.primaryVlanEnable;
        primaryVlanId = builder.primaryVlanId;
        primaryDns = builder.primaryDns;
        secondaryTargetName = builder.secondaryTargetName;
        secondaryTargetIpAddress = builder.secondaryTargetIpAddress;
        secondaryTargetTcpPort = builder.secondaryTargetTcpPort;
        secondaryLun = builder.secondaryLun;
        secondaryVlanEnable = builder.secondaryVlanEnable;
        secondaryVlanId = builder.secondaryVlanId;
        secondaryDns = builder.secondaryDns;
        ipMaskDnsViaDhcp = builder.ipMaskDnsViaDhcp;
        routerAdvertisementEnabled = builder.routerAdvertisementEnabled;
        authenticationMethod = builder.authenticationMethod;
        chapUsername = builder.chapUsername;
        chapSecret = builder.chapSecret;
        mutualChapUsername = builder.mutualChapUsername;
        mutualChapSecret = builder.mutualChapSecret;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public IpAddressType getIpAddressType() {
        return ipAddressType;
    }

    public String getInitiatorIpAddress() {
        return initiatorIpAddress;
    }

    public String getInitiatorName() {
        return initiatorName;
    }

    public String getInitiatorDefaultGateway() {
        return initiatorDefaultGateway;
    }

    public String getInitiatorNetmask() {
        return initiatorNetmask;
    }

    public Boolean getTargetInfoViaDhcp() {
        return targetInfoViaDhcp;
    }

    public String getPrimaryTargetName() {
        return primaryTargetName;
    }

    public String getPrimaryTargetIpAddress() {
        return primaryTargetIpAddress;
    }

    public Integer getPrimaryTargetTcpPort() {
        return primaryTargetTcpPort;
    }

    public Integer getPrimaryLun() {
        return primaryLun;
    }

    public Boolean getPrimaryVlanEnable() {
        return primaryVlanEnable;
    }

    public Integer getPrimaryVlanId() {
        return primaryVlanId;
    }

    public String getPrimaryDns() {
        return primaryDns;
    }

    public String getSecondaryTargetName() {
        return secondaryTargetName;
    }

    public String getSecondaryTargetIpAddress() {
        return secondaryTargetIpAddress;
    }

    public Integer getSecondaryTargetTcpPort() {
        return secondaryTargetTcpPort;
    }

    public Integer getSecondaryLun() {
        return secondaryLun;
    }

    public Boolean getSecondaryVlanEnable() {
        return secondaryVlanEnable;
    }

    public Integer getSecondaryVlanId() {
        return secondaryVlanId;
    }

    public String getSecondaryDns() {
        return secondaryDns;
    }

    public Boolean getIpMaskDnsViaDhcp() {
        return ipMaskDnsViaDhcp;
    }

    public Boolean getRouterAdvertisementEnabled() {
        return routerAdvertisementEnabled;
    }

    public AuthenticationMethod getAuthenticationMethod() {
        return authenticationMethod;
    }

    public String getChapUsername() {
        return chapUsername;
    }

    public String getChapSecret() {
        return chapSecret;
    }

    public String getMutualChapUsername() {
        return mutualChapUsername;
    }

    public String getMutualChapSecret() {
        return mutualChapSecret;
    }

    public static final class Builder {
        private IpAddressType ipAddressType;
        private String initiatorIpAddress;
        private String initiatorName;
        private String initiatorDefaultGateway;
        private String initiatorNetmask;
        private Boolean targetInfoViaDhcp;
        private String primaryTargetName;
        private String primaryTargetIpAddress;
        private Integer primaryTargetTcpPort;
        private Integer primaryLun;
        private Boolean primaryVlanEnable;
        private Integer primaryVlanId;
        private String primaryDns;
        private String secondaryTargetName;
        private String secondaryTargetIpAddress;
        private Integer secondaryTargetTcpPort;
        private Integer secondaryLun;
        private Boolean secondaryVlanEnable;
        private Integer secondaryVlanId;
        private String secondaryDns;
        private Boolean ipMaskDnsViaDhcp;
        private Boolean routerAdvertisementEnabled;
        private AuthenticationMethod authenticationMethod;
        private String chapUsername;
        private String chapSecret;
        private String mutualChapUsername;
        private String mutualChapSecret;

        private Builder() {
        }

        public Builder ipAddressType(IpAddressType ipAddressType) {
            this.ipAddressType = ipAddressType;
            return this;
        }

        public Builder initiatorIpAddress(String initiatorIpAddress) {
            this.initiatorIpAddress = initiatorIpAddress;
            return this;
        }

        public Builder initiatorName(String initiatorName) {
            this.initiatorName = initiatorName;
            return this;
        }

        public Builder initiatorDefaultGateway(String initiatorDefaultGateway) {
            this.initiatorDefaultGateway = initiatorDefaultGateway;
            return this;
        }

        public Builder initiatorNetmask(String initiatorNetmask) {
            this.initiatorNetmask = initiatorNetmask;
            return this;
        }

        public Builder targetInfoViaDhcp(Boolean targetInfoViaDhcp) {
            this.targetInfoViaDhcp = targetInfoViaDhcp;
            return this;
        }

        public Builder primaryTargetName(String primaryTargetName) {
            this.primaryTargetName = primaryTargetName;
            return this;
        }

        public Builder primaryTargetIpAddress(String primaryTargetIpAddress) {
            this.primaryTargetIpAddress = primaryTargetIpAddress;
            return this;
        }

        public Builder primaryTargetTcpPort(Integer primaryTargetTcpPort) {
            this.primaryTargetTcpPort = primaryTargetTcpPort;
            return this;
        }

        public Builder primaryLun(Integer primaryLun) {
            this.primaryLun = primaryLun;
            return this;
        }

        public Builder primaryVlanEnable(Boolean primaryVlanEnable) {
            this.primaryVlanEnable = primaryVlanEnable;
            return this;
        }

        public Builder primaryVlanId(Integer primaryVlanId) {
            this.primaryVlanId = primaryVlanId;
            return this;
        }

        public Builder primaryDns(String primaryDns) {
            this.primaryDns = primaryDns;
            return this;
        }

        public Builder secondaryTargetName(String secondaryTargetName) {
            this.secondaryTargetName = secondaryTargetName;
            return this;
        }

        public Builder secondaryTargetIpAddress(String secondaryTargetIpAddress) {
            this.secondaryTargetIpAddress = secondaryTargetIpAddress;
            return this;
        }

        public Builder secondaryTargetTcpPort(Integer secondaryTargetTcpPort) {
            this.secondaryTargetTcpPort = secondaryTargetTcpPort;
            return this;
        }

        public Builder secondaryLun(Integer secondaryLun) {
            this.secondaryLun = secondaryLun;
            return this;
        }

        public Builder secondaryVlanEnable(Boolean secondaryVlanEnable) {
            this.secondaryVlanEnable = secondaryVlanEnable;
            return this;
        }

        public Builder secondaryVlanId(Integer secondaryVlanId) {
            this.secondaryVlanId = secondaryVlanId;
            return this;
        }

        public Builder secondaryDns(String secondaryDns) {
            this.secondaryDns = secondaryDns;
            return this;
        }

        public Builder ipMaskDnsViaDhcp(Boolean ipMaskDnsViaDhcp) {
            this.ipMaskDnsViaDhcp = ipMaskDnsViaDhcp;
            return this;
        }

        public Builder routerAdvertisementEnabled(Boolean routerAdvertisementEnabled) {
            this.routerAdvertisementEnabled = routerAdvertisementEnabled;
            return this;
        }

        public Builder authenticationMethod(AuthenticationMethod authenticationMethod) {
            this.authenticationMethod = authenticationMethod;
            return this;
        }

        public Builder chapUsername(String chapUsername) {
            this.chapUsername = chapUsername;
            return this;
        }

        public Builder chapSecret(String chapSecret) {
            this.chapSecret = chapSecret;
            return this;
        }

        public Builder mutualChapUsername(String mutualChapUsername) {
            this.mutualChapUsername = mutualChapUsername;
            return this;
        }

        public Builder mutualChapSecret(String mutualChapSecret) {
            this.mutualChapSecret = mutualChapSecret;
            return this;
        }

        public IscsiBootDto build() {
            return new IscsiBootDto(this);
        }
    }
}
