/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier.unification.converters;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper;

import java.net.URI;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.lang.String.format;
import static org.springframework.web.util.UriComponentsBuilder.fromPath;

class TextNodeConverter implements JsonNodeConverter {

    private final Pattern odataIdPattern = Pattern.compile("/redfish/v1[^'{} ]+");

    @Override
    public JsonNode convert(String propertyName, JsonNode jsonNode, OdataIdMapper mapper) {
        Matcher matcher = odataIdPattern.matcher(jsonNode.asText());
        StringBuffer stringBuffer = new StringBuffer();
        while (matcher.find()) {
            URI odataid = URI.create(matcher.group());
            String mappedPath = mapper.mapOdataId(odataid.getPath())
                .map(mp -> fromPath(mp).fragment(odataid.getFragment()).build().toUriString())
                .orElse(format("NOT RESOLVED: %s", matcher.group()));
            matcher.appendReplacement(stringBuffer, mappedPath);
        }
        matcher.appendTail(stringBuffer);
        return new TextNode(stringBuffer.toString());
    }
}
