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

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

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
    @SuppressWarnings("checkstyle:CyclomaticComplexity")
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        MemoryLocation that = (MemoryLocation) o;
        return Objects.equals(socket, that.socket)
                && Objects.equals(memoryController, that.memoryController)
                && Objects.equals(channel, that.channel)
                && Objects.equals(slot, that.slot);
    }

    @Override
    public int hashCode() {
        return Objects.hash(socket, memoryController, channel, slot);
    }
}
