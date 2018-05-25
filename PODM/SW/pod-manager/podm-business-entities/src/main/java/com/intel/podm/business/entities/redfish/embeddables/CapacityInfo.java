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

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;
import java.util.Objects;

@Embeddable
public class CapacityInfo {
    @Column(name = "consumed_bytes")
    private BigDecimal consumedBytes;

    @Column(name = "allocated_bytes")
    private BigDecimal allocatedBytes;

    @Column(name = "guaranteed_bytes")
    private BigDecimal guaranteedBytes;

    @Column(name = "provisioned_bytes")
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

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        CapacityInfo that = (CapacityInfo) o;

        return new EqualsBuilder()
            .append(consumedBytes, that.consumedBytes)
            .append(allocatedBytes, that.allocatedBytes)
            .append(guaranteedBytes, that.guaranteedBytes)
            .append(provisionedBytes, that.provisionedBytes)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(consumedBytes, allocatedBytes, guaranteedBytes, provisionedBytes);
    }
}
