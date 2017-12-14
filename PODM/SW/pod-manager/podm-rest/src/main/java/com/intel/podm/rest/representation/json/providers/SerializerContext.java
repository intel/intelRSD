/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.rest.representation.json.providers;

import com.intel.podm.rest.representation.json.serializers.DtoSerializerContext;

import javax.ws.rs.core.PathSegment;
import java.net.URI;
import java.util.StringJoiner;

public final class SerializerContext implements DtoSerializerContext {
    private JsonProvider jsonProvider;

    SerializerContext(JsonProvider jsonProvider) {
        this.jsonProvider = jsonProvider;
    }

    @Override
    public URI getRequestPath() {
        StringJoiner joiner = new StringJoiner("/", "/", "");
        jsonProvider.uriInfo.getPathSegments().stream()
            .map(PathSegment::getPath)
            .forEach(joiner::add);
        return URI.create(joiner.toString());
    }
}
