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

package com.intel.rsd.resourcemanager.layers.completer;

import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.common.services.ApiCrawlingTask;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;

import static java.util.stream.Collectors.toMap;

@Component
public class CrawledResourcesDistributor implements ApiCrawlingTask.ResourceCollectionConsumer {
    private final Collection<ResourceCompleter> resourceCompleters;

    @Autowired
    public CrawledResourcesDistributor(Collection<ResourceCompleter> resourceCompleters) {
        this.resourceCompleters = Collections.unmodifiableCollection(resourceCompleters);
    }

    @Override
    public void consume(Collection<Resource> resources) {
        val packagesForCompleters = resourceCompleters.stream().collect(toMap(o -> o, o -> new HashSet<Resource>()));
        for (Resource resource : resources) {
            String odataId = resource.getOdataId();
            packagesForCompleters.entrySet().stream()
                .filter(entry -> isCompleterInterestedInResource(entry.getKey(), odataId))
                .forEach(entry -> entry.getValue().add(resource));
        }

        for (val entry : packagesForCompleters.entrySet()) {
            entry.getKey().onResourcesCrawled(entry.getValue());
        }
    }

    private boolean isCompleterInterestedInResource(ResourceCompleter resourceCompleter, String odataId) {
        return resourceCompleter.interestedInPatterns().stream()
            .filter(Objects::nonNull)
            .anyMatch(p -> p.matcher(odataId).matches());
    }
}
