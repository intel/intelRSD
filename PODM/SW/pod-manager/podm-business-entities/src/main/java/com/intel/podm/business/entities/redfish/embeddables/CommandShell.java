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

import com.intel.podm.common.types.GeneralConnectType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.OrderColumn;
import java.util.ArrayList;
import java.util.List;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class CommandShell {
    @Column(name = "service_enabled")
    private Boolean serviceEnabled;

    @Column(name = "max_concurrent_sessions")
    private Integer maxConcurrentSessions;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "command_shell_connect_type_supported", joinColumns = @JoinColumn(name = "command_shell_id"))
    @Column(name = "connect_type_supported")
    @OrderColumn(name = "connect_type_supported_order")
    private List<GeneralConnectType> connectTypesSupported = new ArrayList<>();

    public Boolean isServiceEnabled() {
        return serviceEnabled;
    }

    public void setServiceEnabled(Boolean serviceEnabled) {
        this.serviceEnabled = serviceEnabled;
    }

    public Integer getMaxConcurrentSessions() {
        return maxConcurrentSessions;
    }

    public void setMaxConcurrentSessions(Integer maxConcurrentSessions) {
        this.maxConcurrentSessions = maxConcurrentSessions;
    }

    public List<GeneralConnectType> getConnectTypesSupported() {
        return connectTypesSupported;
    }

    public void addConnectTypesSupported(GeneralConnectType connectTypesSupported) {
        this.connectTypesSupported.add(connectTypesSupported);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        CommandShell that = (CommandShell) o;

        return new EqualsBuilder()
            .append(serviceEnabled, that.serviceEnabled)
            .append(maxConcurrentSessions, that.maxConcurrentSessions)
            .append(connectTypesSupported, that.connectTypesSupported)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(serviceEnabled)
            .append(maxConcurrentSessions)
            .append(connectTypesSupported)
            .toHashCode();
    }
}
