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

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.math.BigDecimal;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({"consumedBytes", "allocatedBytes", "guaranteedBytes", "provisionedBytes"})
@JsonInclude(NON_NULL)
public class CapacityInfoDto {
    private BigDecimal consumedBytes;
    private BigDecimal allocatedBytes;
    private BigDecimal guaranteedBytes;
    private BigDecimal provisionedBytes;

    public BigDecimal getConsumedBytes() {
        return consumedBytes;
    }

    public void setConsumedBytes(BigDecimal consumedBytes) {
        this.consumedBytes = consumedBytes;
    }

    public BigDecimal getAllocatedBytes() {
        return allocatedBytes;
    }

    public void setAllocatedBytes(BigDecimal allocatedBytes) {
        this.allocatedBytes = allocatedBytes;
    }

    public BigDecimal getGuaranteedBytes() {
        return guaranteedBytes;
    }

    public void setGuaranteedBytes(BigDecimal guaranteedBytes) {
        this.guaranteedBytes = guaranteedBytes;
    }

    public BigDecimal getProvisionedBytes() {
        return provisionedBytes;
    }

    public void setProvisionedBytes(BigDecimal provisionedBytes) {
        this.provisionedBytes = provisionedBytes;
    }
}
