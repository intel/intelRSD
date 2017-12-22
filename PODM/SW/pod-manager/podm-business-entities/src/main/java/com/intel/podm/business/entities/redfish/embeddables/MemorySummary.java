/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.common.types.Status;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.math.BigDecimal;
import java.util.Objects;

@Embeddable
public class MemorySummary {
    @Column(name = "memory_summary_status")
    private Status status;

    @Column(name = "memory_summary_total_system_memory_gib")
    private BigDecimal totalSystemMemoryGiB;

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public BigDecimal getTotalSystemMemoryGiB() {
        return totalSystemMemoryGiB;
    }

    public void setTotalSystemMemoryGiB(BigDecimal totalSystemMemoryGiB) {
        this.totalSystemMemoryGiB = totalSystemMemoryGiB;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof MemorySummary)) {
            return false;
        }
        MemorySummary that = (MemorySummary) o;
        return Objects.equals(getStatus(), that.getStatus())
            && Objects.equals(getTotalSystemMemoryGiB(), that.getTotalSystemMemoryGiB());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getStatus(), getTotalSystemMemoryGiB());
    }
}
