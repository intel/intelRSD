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

import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.tagger.data.ResourceReference;
import com.intel.rsd.resourcemanager.layers.tagger.data.ResourceTaggedValues;
import com.intel.rsd.resourcemanager.layers.tagger.data.TagsPersistentRepository;
import com.intel.rsd.services.common.Shared;
import lombok.val;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.HashMap;
import java.util.Map;

import static com.intel.rsd.resourcemanager.layers.tagger.TagBundle.newTagBundleFrom;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
class TagsRepository {
    private final Map<String, Map<String, String>> resourceTagsCache;
    private final TagsPersistentRepository persistentRepository;

    @SuppressWarnings({"unchecked"})
    TagsRepository(@Shared("TagsRepository.resourceTagsCache") Map<?, ?> resourceTagsCache, TagsPersistentRepository persistentRepository) {
        this.persistentRepository = persistentRepository;
        this.resourceTagsCache = (Map<String, Map<String, String>>) resourceTagsCache;
        populateCacheFromRepository();
    }

    private void populateCacheFromRepository() {
        val all = persistentRepository.findAll();
        for (ResourceTaggedValues resourceTaggedValues : all) {
            setAssociatedTaggedValues(resourceTaggedValues.getResourceReference(), resourceTaggedValues.getTaggedValues());
        }
    }

    TagBundle getTagValues(ServiceId serviceId, String path) {
        val resourceId = new ResourceReference(serviceId, path);
        val existingTagsStringRepresentation = resourceTagsCache.getOrDefault(resourceId.asResourceIdString(), new HashMap<>());
        return newTagBundleFrom(existingTagsStringRepresentation);
    }

    void setTagValues(ServiceId serviceId, String path, TagBundle newTagValues) {
        val resourceId = new ResourceReference(serviceId, path);

        val existingTags = getTagValues(serviceId, path);
        val mergedTags = existingTags.copyMerge(newTagValues);

        if (mergedTags.isEmpty()) {
            removeAssociatedTaggedValues(resourceId);
        } else {
            setAssociatedTaggedValues(resourceId, mergedTags);
        }
    }

    private void setAssociatedTaggedValues(ResourceReference resourceId, TagBundle tagBundle) {
        val value = tagBundle.asStringStringMap();

        // execution order matters - first ensure the value is persisted, then populated to tags cache
        persistentRepository.updateOrCreate(resourceId, value);
        resourceTagsCache.put(resourceId.asResourceIdString(), value);
    }

    private void removeAssociatedTaggedValues(ResourceReference resourceId) {
        // execution order matters - first ensure the value is removed from persistent repository, then from cache
        persistentRepository.findById(resourceId).ifPresent(persistentRepository::delete);
        resourceTagsCache.remove(resourceId.asResourceIdString());
    }
}
