/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.crawler.matcher;

import lombok.NonNull;
import org.springframework.util.AntPathMatcher;

import java.util.Collection;

public class AllowResourcesUsingPatternsUriMatcher implements ResourceUriMatcher {
    private final Collection<String> allowablePatterns;
    private final AntPathMatcher antPathMatcher;

    public AllowResourcesUsingPatternsUriMatcher(@NonNull Collection<String> allowablePatterns) {
        this.allowablePatterns = allowablePatterns;
        this.antPathMatcher = new AntPathMatcher();
    }

    @Override
    public boolean uriMatches(String uriAsString) {
        return allowablePatterns.stream()
            .anyMatch(pattern -> antPathMatcher.match(pattern, uriAsString));
    }
}
