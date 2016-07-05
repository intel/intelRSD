/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.rest.odataid;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.types.Id;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

/**
 * OData ids of resources accessible from rest.
 */
public final class ODataIds {
    private static final ContextTypeToResourceNameMapper RESOURCE_NAME_MAPPER = new ContextTypeToResourceNameMapper();

    private ODataIds() {
    }

    public static ODataId oDataIdFromContext(Context context) {
        return context == null
                ? null
                : buildODataId(context);
    }

    public static List<ODataId> oDataIdsCollection(Context... contexts) {
        return Arrays.asList(contexts).stream()
                .filter(Objects::nonNull)
                .map(ODataIds::oDataIdFromContext)
                .collect(toList());
    }

    public static Collection<ODataId> oDataIdsCollection(Collection<Context> contexts) {
        return isNull(contexts) ? emptyList() : oDataIdsCollection(contexts.stream().toArray(Context[]::new));
    }

    public static ODataId oDataIdOfCollectionInContext(Context context, ContextType collectionType) {
        String resourceName = RESOURCE_NAME_MAPPER.get(collectionType);
        return oDataIdOfResourceInContext(context, resourceName);
    }

    public static ODataId oDataIdOfResourceInContext(Context context, String resourceName) {
        return oDataId(format("%s/%s", buildODataId(context), resourceName));
    }

    public static ODataId oDataIdOfServiceRoot() {
        return oDataId("/redfish/v1");
    }

    public static ODataId oDataIdOfRedfishServiceRoot() {
        return oDataId("/redfish/v1");
    }

    public static ODataId oDataIdOfSystems() {
        return oDataId(oDataIdOfServiceRoot() + "/Systems");
    }

    public static ODataId oDataIdOfServices() {
        return oDataId(oDataIdOfServiceRoot() + "/Services");
    }

    private static ODataId build(ODataId parent, ContextType type, Id id) {
        String resourceName = RESOURCE_NAME_MAPPER.get(type);
        return oDataId(format("%s/%s/%s", parent, resourceName, id));
    }

    public static ODataId buildODataId(Context context) {
        if (context == null) {
            return oDataIdOfServiceRoot();
        }

        ODataId parent = context.getParent() == null
                ? oDataIdOfServiceRoot()
                : buildODataId(context.getParent());

        ContextType type = context.getType();
        Id id = context.getId();

        return build(parent, type, id);
    }
}
