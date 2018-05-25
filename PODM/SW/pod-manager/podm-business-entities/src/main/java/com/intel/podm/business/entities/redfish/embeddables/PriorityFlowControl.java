/*
 * Copyright (c) 2018 Intel Corporation
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
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embeddable;
import javax.persistence.JoinColumn;
import javax.persistence.OrderColumn;
import java.util.ArrayList;
import java.util.List;

@Embeddable
public class PriorityFlowControl {
    @Column(name = "priority_flow_control_enabled")
    private Boolean enabled;

    @ElementCollection
    @CollectionTable(name = "ethernet_switch_port_enabled_priorities", joinColumns = @JoinColumn(name = "ethernet_switch_port_id"))
    @Column(name = "enabled_priorities")
    @OrderColumn(name = "enabled_priorities_order")
    private List<Integer> enabledPriorities = new ArrayList<>();

    public Boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(Boolean enabled) {
        this.enabled = enabled;
    }

    public List<Integer> getEnabledPriorities() {
        return enabledPriorities;
    }

    public void addEnabledPriority(Integer enabledPriority) {
        this.enabledPriorities.add(enabledPriority);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        PriorityFlowControl that = (PriorityFlowControl) o;

        return new EqualsBuilder()
            .append(enabled, that.enabled)
            .append(enabledPriorities, that.enabledPriorities)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(enabled)
            .append(enabledPriorities)
            .toHashCode();
    }
}
