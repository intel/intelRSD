/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration.difference;

import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import org.apache.commons.lang3.builder.ToStringBuilder;

import java.util.Objects;

class ConnectedEntityEquator implements EquatorWithHashBuilder<ConnectedEntity> {
    @Override
    public boolean equate(ConnectedEntity first, ConnectedEntity second) {
        return Objects.equals(first.getEntityRole(), second.getEntityRole());
    }

    @Override
    public int hash(ConnectedEntity object) {
        return Objects.hash(object.getEntityRole());
    }

    @Override
    public String asEquatorString(ConnectedEntity connectedEntity) {
        if (connectedEntity != null) {
            return new ToStringBuilder(connectedEntity)
                .append("entityRole", connectedEntity.getEntityRole())
                .toString();
        }
        return null;
    }
}
