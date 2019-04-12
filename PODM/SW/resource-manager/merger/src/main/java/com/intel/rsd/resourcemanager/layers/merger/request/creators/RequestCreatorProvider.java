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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.annotation.AnnotationAwareOrderComparator;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;

import static java.util.stream.Collectors.toList;

@Slf4j
@Component
public class RequestCreatorProvider {
    private Collection<SelectableRequestCreator> selectableRequestCreators;

    @Autowired
    RequestCreatorProvider(Collection<SelectableRequestCreator> selectableRequestCreators) {
        this.selectableRequestCreators = selectableRequestCreators.stream().sorted(new AnnotationAwareOrderComparator().reversed()).collect(toList());
    }

    public RequestCreator selectCreator(String path, HttpMethod httpMethod) {
        return selectableRequestCreators.stream()
            .filter(requestCreator -> requestCreator.canBeSelected(httpMethod, path))
            .peek(requestCreator -> log.debug(
                "{} selected for processing ({}) request on path ({})", requestCreator.getClass().getSimpleName(), httpMethod, path)
            )
            .findFirst()
            .map(RequestCreator.class::cast)
            .orElseThrow(() -> new UnsupportedOperationException("Cannot find creator for requested path and method: " + path + ", " + httpMethod));
    }
}
