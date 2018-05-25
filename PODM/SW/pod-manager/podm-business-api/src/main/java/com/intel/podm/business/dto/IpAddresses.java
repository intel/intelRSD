/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV4AddressOrigin;
import com.intel.podm.common.types.IpV6AddressOrigin;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

public final class IpAddresses {
    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "subnetMask", "addressOrigin", "gateway", "oem"})
    public static final class IpV4AddressDto {
        private String address;
        private String subnetMask;
        private IpV4AddressOrigin addressOrigin;
        private String gateway;
        @IgnoreAutomaticOem
        private JsonNode oem;

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }

        public String getSubnetMask() {
            return subnetMask;
        }

        public void setSubnetMask(String subnetMask) {
            this.subnetMask = subnetMask;
        }

        public IpV4AddressOrigin getAddressOrigin() {
            return addressOrigin;
        }

        public void setAddressOrigin(IpV4AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
        }

        public String getGateway() {
            return gateway;
        }

        public void setGateway(String gateway) {
            this.gateway = gateway;
        }

        public JsonNode getOem() {
            return oem;
        }

        public void setOem(JsonNode oem) {
            this.oem = oem;
        }
    }

    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "prefixLength", "addressOrigin", "addressState", "oem"})
    public static final class IpV6AddressDto {
        private String address;
        private Integer prefixLength;
        private IpV6AddressOrigin addressOrigin;
        private AddressState addressState;
        @IgnoreAutomaticOem
        private JsonNode oem;

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }

        public Integer getPrefixLength() {
            return prefixLength;
        }

        public void setPrefixLength(Integer prefixLength) {
            this.prefixLength = prefixLength;
        }

        public IpV6AddressOrigin getAddressOrigin() {
            return addressOrigin;
        }

        public void setAddressOrigin(IpV6AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
        }

        public AddressState getAddressState() {
            return addressState;
        }

        public void setAddressState(AddressState addressState) {
            this.addressState = addressState;
        }

        public JsonNode getOem() {
            return oem;
        }

        public void setOem(JsonNode oem) {
            this.oem = oem;
        }
    }

    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"prefix", "precedence", "label"})
    public static final class IpV6AddressPolicyDto {
        private String prefix;
        private Integer precedence;
        private Integer label;

        public String getPrefix() {
            return prefix;
        }

        public void setPrefix(String prefix) {
            this.prefix = prefix;
        }

        public Integer getPrecedence() {
            return precedence;
        }

        public void setPrecedence(Integer precedence) {
            this.precedence = precedence;
        }

        public Integer getLabel() {
            return label;
        }

        public void setLabel(Integer label) {
            this.label = label;
        }
    }
}
