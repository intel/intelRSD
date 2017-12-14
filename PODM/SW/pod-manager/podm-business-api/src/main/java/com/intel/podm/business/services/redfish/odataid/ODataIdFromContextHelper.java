/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.services.redfish.odataid;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.context.ContextTypeToEmbeddableMapper;
import com.intel.podm.common.types.Id;

import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromString;
import static java.lang.String.format;
import static java.util.Arrays.stream;
import static java.util.Collections.emptyList;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

/**
 * Helper class for Context to ODataId conversions.
 */
public final class ODataIdFromContextHelper {
    private static final ContextTypeToResourceNameMapper RESOURCE_NAME_MAPPER = new ContextTypeToResourceNameMapper();
    private static final ContextTypeToEmbeddableMapper CONTEXT_EMBEDDABLE_MAPPER = new ContextTypeToEmbeddableMapper();

    private ODataIdFromContextHelper() {
    }

    public static ODataId asOdataId(Context context) {
        return context == null
            ? null
            : buildODataId(context);
    }

    public static Collection<ODataId> asOdataIdCollection(Collection<Context> contexts) {
        return contexts == null ? emptyList() : asOdataIdList(contexts.stream().toArray(Context[]::new));
    }

    public static List<ODataId> asOdataIdList(List<Context> contexts) {
        return contexts == null ? emptyList() : asOdataIdList(contexts.stream().toArray(Context[]::new));
    }

    private static List<ODataId> asOdataIdList(Context... contexts) {
        return stream(contexts)
            .filter(Objects::nonNull)
            .map(ODataIdFromContextHelper::asOdataId)
            .collect(toList());
    }

    public static Set<ODataId> asOdataIdSet(Set<Context> contexts) {
        if (contexts == null) {
            return emptySet();
        } else {
            return contexts.stream()
                .filter(Objects::nonNull)
                .map(ODataIdFromContextHelper::asOdataId)
                .collect(toSet());
        }
    }

    public static ODataId oDataIdOfCollectionInContext(Context context, ContextType collectionType) {
        String resourceName = RESOURCE_NAME_MAPPER.get(collectionType);
        return oDataIdOfResourceInContext(context, resourceName);
    }

    public static ODataId oDataIdOfResourceInContext(Context context, String resourceName) {
        return oDataIdFromString(format("%s/%s", buildODataId(context), resourceName));
    }

    public static ODataId oDataIdOfServiceRoot() {
        return oDataIdFromString("/redfish/v1");
    }

    private static ODataId build(ODataId parent, ContextType type, Id id) {
        String resourceName = RESOURCE_NAME_MAPPER.get(type);
        return oDataIdFromString(format(CONTEXT_EMBEDDABLE_MAPPER.isEmbeddable(type) ? "%s#/%s/%s" : "%s/%s/%s", parent, resourceName, id));
    }

    private static ODataId buildODataId(Context context) {
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
