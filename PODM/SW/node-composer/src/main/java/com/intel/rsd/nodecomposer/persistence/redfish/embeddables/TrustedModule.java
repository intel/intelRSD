/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.types.InterfaceType;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

import static javax.persistence.EnumType.STRING;

@EqualsAndHashCode
@Embeddable
public class TrustedModule {
    @Getter
    @Setter
    @Column(name = "trusted_module_firmware_version")
    private String firmwareVersion;

    @Getter
    @Setter
    @Column(name = "trusted_module_interface_type")
    @Enumerated(STRING)
    private InterfaceType interfaceType;

    @Getter
    @Setter
    @Column(name = "trusted_module_status")
    private Status status;
}
