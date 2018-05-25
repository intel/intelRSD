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

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

@Embeddable
public class RedfishLocation {
    @Column(name = "info")
    private String info;

    @Column(name = "info_format")
    private String infoFormat;

    public String getInfo() {
        return info;
    }

    public void setInfo(String info) {
        this.info = info;
    }

    public String getInfoFormat() {
        return infoFormat;
    }

    public void setInfoFormat(String infoFormat) {
        this.infoFormat = infoFormat;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        RedfishLocation that = (RedfishLocation) o;
        return Objects.equals(info, that.info)
                && Objects.equals(infoFormat, that.infoFormat);
    }

    @Override
    public int hashCode() {
        return Objects.hash(info, infoFormat);
    }
}
