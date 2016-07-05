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

package com.intel.podm.repository.api;

import java.util.Map;

public class GraphVertexType {
    private final String vertexTypeName;
    private final Map<String, Class> vertexProperties;

    public GraphVertexType(String vertexTypeName, Map<String, Class> vertexProperties) {
        if (vertexTypeName == null) {
            throw new IllegalArgumentException("Argument vertexTypeName should not be null");
        }
        if (vertexTypeName.isEmpty()) {
            throw new IllegalArgumentException("Argument vertexTypeName should not be an empty string");
        }
        if (vertexProperties == null) {
            throw new IllegalArgumentException("Argument vertexProperties should not be null");
        }
        this.vertexTypeName = vertexTypeName;
        this.vertexProperties = vertexProperties;
    }

    public String getVertexTypeName() {
        return vertexTypeName;
    }

    public Map<String, Class> getVertexProperties() {
        return vertexProperties;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (!(o instanceof GraphVertexType)) {
            return false;
        }

        GraphVertexType that = (GraphVertexType) o;
        if (!vertexTypeName.equals(that.vertexTypeName)) {
            return false;
        }

        return vertexProperties.equals(that.vertexProperties);
    }

    @Override
    public int hashCode() {
        return vertexTypeName.hashCode() + vertexProperties.hashCode();
    }
}
