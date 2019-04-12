/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish.embeddables;

import com.intel.rsd.nodecomposer.types.AddressState;
import com.intel.rsd.nodecomposer.types.IpV6AddressOrigin;
import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

import static javax.persistence.EnumType.STRING;

@Getter
@Setter
@EqualsAndHashCode
@Embeddable
public class IpV6Address {
    @Column(name = "address")
    private String address;

    @Column(name = "prefix_length")
    private Integer prefixLength;

    @Column(name = "address_origin")
    @Enumerated(STRING)
    private IpV6AddressOrigin addressOrigin;

    @Column(name = "address_state")
    @Enumerated(STRING)
    private AddressState addressState;

    @Column(name = "oem")
    private String oem;
}
