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

import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import org.apache.commons.lang3.builder.ToStringBuilder;

import java.util.Objects;

class IdentifierEquator implements EquatorWithHashBuilder<Identifier> {
    @Override
    public boolean equate(Identifier first, Identifier second) {
        return Objects.equals(first.getDurableName(), second.getDurableName())
            && Objects.equals(first.getDurableNameFormat(), second.getDurableNameFormat());
    }

    @Override
    public int hash(Identifier object) {
        return Objects.hash(object.getDurableName(), object.getDurableNameFormat());
    }

    @Override
    public String asEquatorString(Identifier identifier) {
        if (identifier != null) {
            return new ToStringBuilder(identifier)
                .append("DurableName", identifier.getDurableName())
                .append("DurableNameFormat", identifier.getDurableNameFormat())
                .toString();
        }
        return null;
    }
}
