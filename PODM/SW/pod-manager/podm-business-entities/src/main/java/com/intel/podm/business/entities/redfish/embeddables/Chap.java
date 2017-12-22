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

import com.intel.podm.common.types.ChapType;
import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class Chap {
    @Column(name = "type")
    @Enumerated(STRING)
    private ChapType type;

    @Column(name = "username")
    private String username;

    @Column(name = "mutual_username")
    private String mutualUsername;

    public ChapType getType() {
        return type;
    }

    public void setType(ChapType type) {
        this.type = type;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getMutualUsername() {
        return mutualUsername;
    }

    public void setMutualUsername(String mutualUsername) {
        this.mutualUsername = mutualUsername;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        Chap that = (Chap) o;
        return new EqualsBuilder()
            .append(type, that.type)
            .append(username, that.username)
            .append(mutualUsername, that.mutualUsername)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(type, username, mutualUsername);
    }
}
