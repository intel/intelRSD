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

package com.intel.rsd.resourcemanager.layers.filter;

import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.NonNull;
import lombok.val;
import org.springframework.web.util.UriTemplate;

import java.net.URI;

final class FilterPattern {
    private final UriTemplate uriTemplate;
    private final JsonPropertyFilter filter;

    FilterPattern(@NonNull String pattern) {
        val uri = URI.create(pattern);
        uriTemplate = new UriTemplate(uri.getPath().replace("*", "{}"));
        filter = new JsonPropertyFilter(uri.getFragment());
    }

    void process(@NonNull String uri, @NonNull ObjectNode node) {
        if (uriTemplate.matches(uri)) {
            filter.filter(node);
        }
    }
}
