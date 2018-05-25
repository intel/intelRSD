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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;

@Embeddable
public class MemoryLocation {
    @Column(name = "socket")
    private Integer socket;

    @Column(name = "memory_controller")
    private Integer memoryController;

    @Column(name = "channel")
    private Integer channel;

    @Column(name = "slot")
    private Integer slot;

    public Integer getSocket() {
        return socket;
    }

    public void setSocket(Integer socket) {
        this.socket = socket;
    }

    public Integer getMemoryController() {
        return memoryController;
    }

    public void setMemoryController(Integer memoryController) {
        this.memoryController = memoryController;
    }

    public Integer getChannel() {
        return channel;
    }

    public void setChannel(Integer channel) {
        this.channel = channel;
    }

    public Integer getSlot() {
        return slot;
    }

    public void setSlot(Integer slot) {
        this.slot = slot;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        MemoryLocation that = (MemoryLocation) o;

        return new EqualsBuilder()
            .append(socket, that.socket)
            .append(memoryController, that.memoryController)
            .append(channel, that.channel)
            .append(slot, that.slot)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(socket)
            .append(memoryController)
            .append(channel)
            .append(slot)
            .toHashCode();
    }
}
