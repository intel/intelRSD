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

package com.intel.rsd.resourcemanager.layers.tagger;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.util.AntPathMatcher;

import java.util.ArrayList;
import java.util.List;

@Component
public class TagDefinitionsProvider {
    private final TaggerConfig taggerConfig;
    private final AntPathMatcher antPathMatcher;

    @Autowired
    public TagDefinitionsProvider(TaggerConfig taggerConfig) {
        this.taggerConfig = taggerConfig;
        this.antPathMatcher = new AntPathMatcher();

        for (TagDefinition tagDefinition : taggerConfig.getTagDefinitions()) {
            if (tagDefinition.getProperty().toString().matches(".*(/[0-9]+).*")) {
                throw new UnsupportedOperationException("JSON Pointers with array segments in path are not supported");
            }
        }
    }

    List<TagDefinition> getTagDefinitionsForPath(String path) {
        List<TagDefinition> tagDefinitionList = new ArrayList<>();
        for (TagDefinition tagDefinition : taggerConfig.getTagDefinitions()) {
            if (antPathMatcher.match(tagDefinition.getResource(), path)) {
                tagDefinitionList.add(tagDefinition);
            }
        }
        return tagDefinitionList;
    }
}
