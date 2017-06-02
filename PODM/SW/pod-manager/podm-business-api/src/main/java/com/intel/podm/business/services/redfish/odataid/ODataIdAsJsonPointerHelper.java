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

package com.intel.podm.business.services.redfish.odataid;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.context.ContextTypeToEmbeddableMapper;
import com.intel.podm.business.services.context.ContextTypeToResourceNameMapper;
import com.intel.podm.common.types.Id;

import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.oDataIdOfServiceRoot;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromString;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

/**
 * Helper class for Context to ODataId as JsonPointer conversions.
 */
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ODataIdAsJsonPointerHelper {
    private static final ContextTypeToResourceNameMapper RESOURCE_NAME_MAPPER = new ContextTypeToResourceNameMapper();
    private static final ContextTypeToEmbeddableMapper CONTEXT_EMBEDDABLE_MAPPER = new ContextTypeToEmbeddableMapper();

    private ODataIdAsJsonPointerHelper() {
    }

    public static ODataId asJsonPointerOdataId(Context context, String jsonPointerId) {
        return context == null
            ? null
            : buildODataId(context, jsonPointerId);
    }

    public static Collection<ODataId> asJsonPointerOdataIdCollection(Collection<Context> contexts, String jsonPointerId) {
        return contexts == null ? emptyList() : asJsonPointerOdataIdList(jsonPointerId, contexts.stream().toArray(Context[]::new));
    }

    private static List<ODataId> asJsonPointerOdataIdList(String jsonPointerId, Context... contexts) {
        return Arrays.stream(contexts)
            .filter(Objects::nonNull)
            .map(oDataId -> asJsonPointerOdataId(oDataId, jsonPointerId))
            .collect(toList());
    }

    public static Set<ODataId> asJsonPointerOdataIdSet(Set<Context> contexts, String jsonPointerId) {
        return contexts == null ? emptySet() : asOdataIdSet(jsonPointerId, contexts.stream().toArray(Context[]::new));
    }

    private static Set<ODataId> asOdataIdSet(String jsonPointerId, Context... contexts) {
        return Arrays.stream(contexts)
            .filter(Objects::nonNull)
            .map(context -> asJsonPointerOdataId(context, jsonPointerId))
            .collect(toSet());
    }

    private static ODataId buildODataId(Context context, String jsonPointerId) {
        if (context == null) {
            return oDataIdOfServiceRoot();
        }

        ODataId parent = context.getParent() == null
            ? oDataIdOfServiceRoot()
            : buildODataId(context.getParent(), jsonPointerId);

        ContextType type = context.getType();
        Id id = context.getId();
        return build(parent, type, id, jsonPointerId);
    }

    private static ODataId build(ODataId parent, ContextType type, Id id, String jsonPointerId) {
        String resourceName = RESOURCE_NAME_MAPPER.get(type);
        return oDataIdFromString(format(CONTEXT_EMBEDDABLE_MAPPER.isEmbeddable(type) ? "%s#/%s/" + jsonPointerId : "%s/%s/%s", parent, resourceName, id));
    }
}
