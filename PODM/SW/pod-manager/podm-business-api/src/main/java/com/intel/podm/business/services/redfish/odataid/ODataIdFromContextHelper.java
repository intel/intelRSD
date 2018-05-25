/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromString;
import static java.lang.String.format;

/**
 * Helper class for Context to ODataId conversions.
 */
public final class ODataIdFromContextHelper {
    private static final ContextTypeToResourceNameMapper RESOURCE_NAME_MAPPER = new ContextTypeToResourceNameMapper();
    private static final ContextTypeToEmbeddableMapper CONTEXT_EMBEDDABLE_MAPPER = new ContextTypeToEmbeddableMapper();

    private ODataIdFromContextHelper() {
    }

    public static ODataId asOdataId(Context context) {
        return context == null ? null : buildODataId(context);
    }

    static ODataId oDataIdOfCollectionInContext(Context context, ContextType collectionType) {
        String resourceName = RESOURCE_NAME_MAPPER.get(collectionType);
        return oDataIdOfResourceInContext(context, resourceName);
    }

    private static ODataId oDataIdOfResourceInContext(Context context, String resourceName) {
        return oDataIdFromString(format("%s/%s", buildODataId(context), resourceName));
    }

    public static ODataId oDataIdOfServiceRoot() {
        return oDataIdFromString("/redfish/v1");
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

    private static ODataId build(ODataId parent, ContextType type, Id id) {
        String resourceName = RESOURCE_NAME_MAPPER.get(type);
        return oDataIdFromString(format(CONTEXT_EMBEDDABLE_MAPPER.isEmbeddable(type) ? "%s#/%s/%s" : "%s/%s/%s", parent, resourceName, id));
    }
}
