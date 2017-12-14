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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

@Embeddable
public class CurrentPeriod {
    @Column(name = "blocks_read")
    private Long blocksRead;

    @Column(name = "blocks_written")
    private Long blocksWritten;

    public Long getBlocksRead() {
        return blocksRead;
    }

    public void setBlocksRead(Long blocksRead) {
        this.blocksRead = blocksRead;
    }

    public Long getBlocksWritten() {
        return blocksWritten;
    }

    public void setBlocksWritten(Long blocksWritten) {
        this.blocksWritten = blocksWritten;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        CurrentPeriod that = (CurrentPeriod) o;
        return new EqualsBuilder()
            .append(blocksRead, that.blocksRead)
            .append(blocksWritten, that.blocksWritten)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(blocksRead, blocksWritten);
    }
}
