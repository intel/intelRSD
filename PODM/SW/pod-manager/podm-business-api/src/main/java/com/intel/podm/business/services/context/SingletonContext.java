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

package com.intel.podm.business.services.context;

import com.intel.podm.business.services.redfish.odataid.ODataIdFromSingletonContextHelper;
import com.intel.podm.common.types.redfish.NavigationProperty;
import com.intel.podm.common.types.redfish.OdataIdProvider;

import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

public final class SingletonContext implements NavigationProperty, OdataIdProvider {
    private final OdataIdProvider ownerContext;
    private final String singletonName;

    private SingletonContext(OdataIdProvider ownerContext, String singletonName) {
        requiresNonNull(ownerContext, "ownerContext");
        requiresNonNull(singletonName, "singletonName");
        this.ownerContext = ownerContext;
        this.singletonName = singletonName;
    }

    public static SingletonContext singletonContextOf(OdataIdProvider ownerContext, String collectionName) {
        return new SingletonContext(ownerContext, collectionName);
    }

    public OdataIdProvider getOwnerODataId() {
        return ownerContext;
    }

    public String getSingletonName() {
        return singletonName;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof SingletonContext)) {
            return false;
        }

        SingletonContext rhs = (SingletonContext) o;
        return Objects.equals(ownerContext, rhs.ownerContext)
            && Objects.equals(singletonName, rhs.singletonName);
    }

    @Override
    public int hashCode() {
        return Objects.hash(ownerContext, singletonName);
    }

    @Override
    public String toString() {
        return format("%s-%s", ownerContext, singletonName);
    }

    @Override
    public ODataId asOdataId() {
        return ODataIdFromSingletonContextHelper.asOdataId(this);
    }
}
