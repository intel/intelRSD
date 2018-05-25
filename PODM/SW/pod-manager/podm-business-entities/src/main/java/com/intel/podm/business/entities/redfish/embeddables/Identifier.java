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

import com.intel.podm.common.types.DurableNameFormat;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class Identifier {
    @Column(name = "durable_name")
    private String durableName;

    @Column(name = "durable_name_format")
    @Enumerated(STRING)
    private DurableNameFormat durableNameFormat;

    public String getDurableName() {
        return durableName;
    }

    public void setDurableName(String durableName) {
        this.durableName = durableName;
    }

    public DurableNameFormat getDurableNameFormat() {
        return durableNameFormat;
    }

    public void setDurableNameFormat(DurableNameFormat durableNameFormat) {
        this.durableNameFormat = durableNameFormat;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        Identifier that = (Identifier) o;
        return Objects.equals(durableName, that.durableName)
            && durableNameFormat == that.durableNameFormat;
    }

    @Override
    public int hashCode() {
        return Objects.hash(durableName, durableNameFormat);
    }
}
