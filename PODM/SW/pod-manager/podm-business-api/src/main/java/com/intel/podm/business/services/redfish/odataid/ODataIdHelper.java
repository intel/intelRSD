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

package com.intel.podm.business.services.redfish.odataid;

import com.intel.podm.common.types.Id;

import java.net.URI;

import static com.google.common.base.Strings.isNullOrEmpty;
import static org.apache.commons.lang3.StringUtils.removeEnd;

public final class ODataIdHelper {
    private ODataIdHelper() {
    }

    public static ODataId oDataIdFromUri(URI uri) {
        URI uriPathWithoutTrailingSlash = URI.create(removeEnd(uri.getPath(), "/"));
        if (!isNullOrEmpty(uri.getFragment())) {
            return new ODataId(URI.create(uriPathWithoutTrailingSlash + "#" + uri.getFragment()));
        }

        return new ODataId(uriPathWithoutTrailingSlash);
    }

    public static ODataId oDataIdFromString(String uriString) {
        return oDataIdFromUri(URI.create(uriString));
    }

    public static ODataId appendOdataId(ODataId oDataId, Id id) {
        String uri = oDataId.toString();
        return oDataIdFromString(uri.endsWith("/") ? (uri + id) : (uri + "/" + id));
    }
}
