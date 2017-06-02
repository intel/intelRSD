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
public class UnknownOem {
    @Column(name = "oem_path")
    private String oemPath;

    @Column(name = "oem_value")
    private String oemValue;

    public String getOemPath() {
        return oemPath;
    }

    public void setOemPath(String oemPath) {
        this.oemPath = oemPath;
    }

    public String getOemValue() {
        return oemValue;
    }

    public void setOemValue(String oemValue) {
        this.oemValue = oemValue;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        UnknownOem that = (UnknownOem) o;
        return Objects.equals(oemPath, that.oemPath)
            && Objects.equals(oemValue, that.oemValue);
    }

    @Override
    public int hashCode() {
        return Objects.hash(oemPath, oemValue);
    }
}
