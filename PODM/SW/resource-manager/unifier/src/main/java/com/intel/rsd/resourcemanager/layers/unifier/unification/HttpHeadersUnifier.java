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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.intel.rsd.collections.Lists;
import lombok.NonNull;
import org.springframework.http.HttpHeaders;
import org.springframework.stereotype.Component;

import java.util.AbstractMap.SimpleEntry;
import java.util.List;
import java.util.Map.Entry;
import java.util.Optional;

import static java.net.URI.create;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;
import static org.springframework.http.HttpHeaders.LOCATION;

@Component
public class HttpHeadersUnifier implements Unifier<HttpHeaders> {

    @Override
    public HttpHeaders unify(@NonNull HttpHeaders toBeUnified, UnifiedOdataIdResolver resolver) {
        return toBeUnified.entrySet().stream()
            .map(header -> unifyHeader(header, resolver))
            .collect(toMap(Entry::getKey, Entry::getValue, Lists::concat, HttpHeaders::new));
    }

    private SimpleEntry<String, List<String>> unifyHeader(Entry<String, List<String>> header, UnifiedOdataIdResolver resolver) {
        switch (header.getKey()) {
            case LOCATION:
                return new SimpleEntry<>(LOCATION, unifyLocationHeader(header.getValue(), resolver));
            default:
                return new SimpleEntry<>(header.getKey(), header.getValue());
        }
    }

    @Override
    public Class<HttpHeaders> getUnifiableClass() {
        return HttpHeaders.class;
    }

    private List<String> unifyLocationHeader(List<String> locationHeaderValues, UnifiedOdataIdResolver resolver) {
        return locationHeaderValues.stream()
            .map(location -> resolver.toUnified(create(location).getPath()))
            .filter(Optional::isPresent)
            .map(Optional::get)
            .collect(toList());
    }
}
