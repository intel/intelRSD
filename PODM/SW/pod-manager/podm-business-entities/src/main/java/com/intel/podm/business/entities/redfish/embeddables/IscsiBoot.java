/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.redfish.embeddables;

import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

import static javax.persistence.EnumType.STRING;

@Embeddable
@SuppressWarnings({"checkstyle:MethodCount"})
public class IscsiBoot {
    @Column(name = "ip_address_type")
    @Enumerated(STRING)
    private IpAddressType ipAddressType;

    @Column(name = "initiator_ip_address")
    private String initiatorIpAddress;

    @Column(name = "initiator_name")
    private String initiatorName;

    @Column(name = "initiator_default_gateway")
    private String initiatorDefaultGateway;

    @Column(name = "initiator_netmask")
    private String initiatorNetmask;

    @Column(name = "target_info_via_dhcp")
    private Boolean targetInfoViaDhcp;

    @Column(name = "primary_target_name")
    private String primaryTargetName;

    @Column(name = "primary_target_ip_address")
    private String primaryTargetIpAddress;

    @Column(name = "primary_target_tcp_port")
    private Integer primaryTargetTcpPort;

    @Column(name = "primary_lun")
    private Integer primaryLun;

    @Column(name = "primary_vlan_enable")
    private Boolean primaryVlanEnable;

    @Column(name = "primary_vlan_id")
    private Integer primaryVlanId;

    @Column(name = "primary_dns")
    private String primaryDns;

    @Column(name = "secondary_target_name")
    private String secondaryTargetName;

    @Column(name = "secondary_target_ip_address")
    private String secondaryTargetIpAddress;

    @Column(name = "secondary_target_tcp_port")
    private Integer secondaryTargetTcpPort;

    @Column(name = "secondary_lun")
    private Integer secondaryLun;

    @Column(name = "secondary_vlan_enable")
    private Boolean secondaryVlanEnable;

    @Column(name = "secondary_vlan_id")
    private Integer secondaryVlanId;

    @Column(name = "secondary_dns")
    private String secondaryDns;

    @Column(name = "ip_mask_dns_via_dhcp")
    private Boolean ipMaskDnsViaDhcp;

    @Column(name = "router_advertisement_enabled")
    private Boolean routerAdvertisementEnabled;

    @Column(name = "authentication_method")
    @Enumerated(STRING)
    private AuthenticationMethod authenticationMethod;

    @Column(name = "chap_username")
    private String chapUsername;

    @Column(name = "mutual_chap_username")
    private String mutualChapUsername;

    public IpAddressType getIpAddressType() {
        return ipAddressType;
    }

    public void setIpAddressType(IpAddressType ipAddressType) {
        this.ipAddressType = ipAddressType;
    }

    public String getInitiatorIpAddress() {
        return initiatorIpAddress;
    }

    public void setInitiatorIpAddress(String initiatorIpAddress) {
        this.initiatorIpAddress = initiatorIpAddress;
    }

    public String getInitiatorName() {
        return initiatorName;
    }

    public void setInitiatorName(String initiatorName) {
        this.initiatorName = initiatorName;
    }

    public String getInitiatorDefaultGateway() {
        return initiatorDefaultGateway;
    }

    public void setInitiatorDefaultGateway(String initiatorDefaultGateway) {
        this.initiatorDefaultGateway = initiatorDefaultGateway;
    }

    public String getInitiatorNetmask() {
        return initiatorNetmask;
    }

    public void setInitiatorNetmask(String initiatorNetmask) {
        this.initiatorNetmask = initiatorNetmask;
    }

    public Boolean getTargetInfoViaDhcp() {
        return targetInfoViaDhcp;
    }

    public void setTargetInfoViaDhcp(Boolean targetInfoViaDhcp) {
        this.targetInfoViaDhcp = targetInfoViaDhcp;
    }

    public String getPrimaryTargetName() {
        return primaryTargetName;
    }

    public void setPrimaryTargetName(String primaryTargetName) {
        this.primaryTargetName = primaryTargetName;
    }

    public String getPrimaryTargetIpAddress() {
        return primaryTargetIpAddress;
    }

    public void setPrimaryTargetIpAddress(String primaryTargetIpAddress) {
        this.primaryTargetIpAddress = primaryTargetIpAddress;
    }

    public Integer getPrimaryTargetTcpPort() {
        return primaryTargetTcpPort;
    }

    public void setPrimaryTargetTcpPort(Integer primaryTargetTcpPort) {
        this.primaryTargetTcpPort = primaryTargetTcpPort;
    }

    public Integer getPrimaryLun() {
        return primaryLun;
    }

    public void setPrimaryLun(Integer primaryLun) {
        this.primaryLun = primaryLun;
    }

    public Boolean getPrimaryVlanEnable() {
        return primaryVlanEnable;
    }

    public void setPrimaryVlanEnable(Boolean primaryVlanEnable) {
        this.primaryVlanEnable = primaryVlanEnable;
    }

    public Integer getPrimaryVlanId() {
        return primaryVlanId;
    }

    public void setPrimaryVlanId(Integer primaryVlanId) {
        this.primaryVlanId = primaryVlanId;
    }

    public String getPrimaryDns() {
        return primaryDns;
    }

    public void setPrimaryDns(String primaryDns) {
        this.primaryDns = primaryDns;
    }

    public String getSecondaryTargetName() {
        return secondaryTargetName;
    }

    public void setSecondaryTargetName(String secondaryTargetName) {
        this.secondaryTargetName = secondaryTargetName;
    }

    public String getSecondaryTargetIpAddress() {
        return secondaryTargetIpAddress;
    }

    public void setSecondaryTargetIpAddress(String secondaryTargetIpAddress) {
        this.secondaryTargetIpAddress = secondaryTargetIpAddress;
    }

    public Integer getSecondaryTargetTcpPort() {
        return secondaryTargetTcpPort;
    }

    public void setSecondaryTargetTcpPort(Integer secondaryTargetTcpPort) {
        this.secondaryTargetTcpPort = secondaryTargetTcpPort;
    }

    public Integer getSecondaryLun() {
        return secondaryLun;
    }

    public void setSecondaryLun(Integer secondaryLun) {
        this.secondaryLun = secondaryLun;
    }

    public Boolean getSecondaryVlanEnable() {
        return secondaryVlanEnable;
    }

    public void setSecondaryVlanEnable(Boolean secondaryVlanEnable) {
        this.secondaryVlanEnable = secondaryVlanEnable;
    }

    public Integer getSecondaryVlanId() {
        return secondaryVlanId;
    }

    public void setSecondaryVlanId(Integer secondaryVlanId) {
        this.secondaryVlanId = secondaryVlanId;
    }

    public String getSecondaryDns() {
        return secondaryDns;
    }

    public void setSecondaryDns(String secondaryDns) {
        this.secondaryDns = secondaryDns;
    }

    public Boolean getIpMaskDnsViaDhcp() {
        return ipMaskDnsViaDhcp;
    }

    public void setIpMaskDnsViaDhcp(Boolean ipMaskDnsViaDhcp) {
        this.ipMaskDnsViaDhcp = ipMaskDnsViaDhcp;
    }

    public Boolean getRouterAdvertisementEnabled() {
        return routerAdvertisementEnabled;
    }

    public void setRouterAdvertisementEnabled(Boolean routerAdvertisementEnabled) {
        this.routerAdvertisementEnabled = routerAdvertisementEnabled;
    }

    public AuthenticationMethod getAuthenticationMethod() {
        return authenticationMethod;
    }

    public void setAuthenticationMethod(AuthenticationMethod authenticationMethod) {
        this.authenticationMethod = authenticationMethod;
    }

    public String getChapUsername() {
        return chapUsername;
    }

    public void setChapUsername(String chapUsername) {
        this.chapUsername = chapUsername;
    }

    public String getMutualChapUsername() {
        return mutualChapUsername;
    }

    public void setMutualChapUsername(String mutualChapUsername) {
        this.mutualChapUsername = mutualChapUsername;
    }

    @Override
    @SuppressWarnings({"checkstyle:MethodLength"})
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        IscsiBoot iscsiBoot = (IscsiBoot) o;
        return new EqualsBuilder()
            .append(ipAddressType, iscsiBoot.ipAddressType)
            .append(initiatorIpAddress, iscsiBoot.initiatorIpAddress)
            .append(initiatorName, iscsiBoot.initiatorName)
            .append(initiatorDefaultGateway, iscsiBoot.initiatorDefaultGateway)
            .append(initiatorNetmask, iscsiBoot.initiatorNetmask)
            .append(targetInfoViaDhcp, iscsiBoot.targetInfoViaDhcp)
            .append(primaryTargetName, iscsiBoot.primaryTargetName)
            .append(primaryTargetIpAddress, iscsiBoot.primaryTargetIpAddress)
            .append(primaryTargetTcpPort, iscsiBoot.primaryTargetTcpPort)
            .append(primaryLun, iscsiBoot.primaryLun)
            .append(primaryVlanEnable, iscsiBoot.primaryVlanEnable)
            .append(primaryVlanId, iscsiBoot.primaryVlanId)
            .append(primaryDns, iscsiBoot.primaryDns)
            .append(secondaryTargetName, iscsiBoot.secondaryTargetName)
            .append(secondaryTargetIpAddress, iscsiBoot.secondaryTargetIpAddress)
            .append(secondaryTargetTcpPort, iscsiBoot.secondaryTargetTcpPort)
            .append(secondaryLun, iscsiBoot.secondaryLun)
            .append(secondaryVlanEnable, iscsiBoot.secondaryVlanEnable)
            .append(secondaryVlanId, iscsiBoot.secondaryVlanId)
            .append(secondaryDns, iscsiBoot.secondaryDns)
            .append(ipMaskDnsViaDhcp, iscsiBoot.ipMaskDnsViaDhcp)
            .append(routerAdvertisementEnabled, iscsiBoot.routerAdvertisementEnabled)
            .append(authenticationMethod, iscsiBoot.authenticationMethod)
            .append(chapUsername, iscsiBoot.chapUsername)
            .append(mutualChapUsername, iscsiBoot.mutualChapUsername)
            .isEquals();
    }

    @Override
    @SuppressWarnings({"checkstyle:MethodLength"})
    public int hashCode() {
        return new HashCodeBuilder()
            .append(ipAddressType)
            .append(initiatorIpAddress)
            .append(initiatorName)
            .append(initiatorDefaultGateway)
            .append(initiatorNetmask)
            .append(targetInfoViaDhcp)
            .append(primaryTargetName)
            .append(primaryTargetIpAddress)
            .append(primaryTargetTcpPort)
            .append(primaryLun)
            .append(primaryVlanEnable)
            .append(primaryVlanId)
            .append(primaryDns)
            .append(secondaryTargetName)
            .append(secondaryTargetIpAddress)
            .append(secondaryTargetTcpPort)
            .append(secondaryLun)
            .append(secondaryVlanEnable)
            .append(secondaryVlanId)
            .append(secondaryDns)
            .append(ipMaskDnsViaDhcp)
            .append(routerAdvertisementEnabled)
            .append(authenticationMethod)
            .append(chapUsername)
            .append(mutualChapUsername)
            .toHashCode();
    }
}
