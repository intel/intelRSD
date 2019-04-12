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

import com.fasterxml.jackson.core.JsonPointer;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.tagger.data.ResourceReference;
import com.intel.rsd.resourcemanager.layers.tagger.data.ResourceTaggedValues;
import com.intel.rsd.resourcemanager.layers.tagger.data.TagsPersistentRepository;
import lombok.val;
import org.testng.annotations.Test;

import javax.persistence.PersistenceException;
import java.util.HashMap;
import java.util.Map;

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static com.intel.rsd.resourcemanager.layers.tagger.TagBundle.newTagBundle;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyMap;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.mockito.Mockito.verifyZeroInteractions;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"unchecked"})
public class TagsRepositoryTest {
    private static final ServiceId SERVICE_ID = new ServiceId("someServiceId");
    private final Map<String, Map<String, String>> resourceTagsCache = new HashMap<>();
    private TagsRepository tagsRepository;

    @Test
    public void uponCreationOfTagsRepository_shouldPopulateItsContentsFromPersistentBackend() {
        val tagsPersistentRepository = mock(TagsPersistentRepository.class);

        resourceTagsCache.clear();
        val tag = "/AssetTag";
        val value = "\"someAssetTag\"";
        val path = "/some/path";
        mockSinglePersistentRepositoryResponse(tagsPersistentRepository, path, tag, value);

        this.tagsRepository = new TagsRepository(resourceTagsCache, tagsPersistentRepository);
        verify(tagsPersistentRepository).findAll();

        val tagValues = tagsRepository.getTagValues(SERVICE_ID, path);
        assertEquals(tagValues.size(), 1);

        val key = JsonPointer.compile(tag);
        assertEquals(stringToJsonNode(value).textValue(), tagValues.get(key).textValue());
    }

    private void mockSinglePersistentRepositoryResponse(TagsPersistentRepository mock, String path, String tag, String value) {
        val tagMap = new HashMap<>();
        tagMap.put(tag, value);

        val entry = new ResourceTaggedValues();
        entry.setResourceReference(new ResourceReference(SERVICE_ID, path));
        entry.setSerializedTaggedValues(objectToJsonNode(tagMap).toString());
        when(mock.findAll()).thenReturn(singletonList(entry));
    }

    @Test
    public void whenRetrievingTagsFromEmptyRepository_shouldReturnEmptyMap() {
        val tagsPersistentRepository = mock(TagsPersistentRepository.class);
        when(tagsPersistentRepository.findAll()).thenReturn(emptyList());
        resourceTagsCache.clear();

        this.tagsRepository = new TagsRepository(resourceTagsCache, tagsPersistentRepository);

        val tagValues = tagsRepository.getTagValues(SERVICE_ID, "/some/path");
        assertTrue(tagValues.isEmpty());
    }

    @Test
    public void whenStoringAndRetrievingTagFromRepository_shouldInteractWithPersistentRepositoryOnlyOnWrite() {
        val tagsPersistentRepository = mock(TagsPersistentRepository.class);
        resourceTagsCache.clear();
        this.tagsRepository = new TagsRepository(resourceTagsCache, tagsPersistentRepository);

        verify(tagsPersistentRepository).findAll();
        verifyZeroInteractions(tagsPersistentRepository);

        val tagValues = newTagBundle();
        val jsonPointer = JsonPointer.compile("/AssetTag");
        val value = stringToJsonNode("\"someValue\"");
        tagValues.put(jsonPointer, value);

        val path = "/some/path";
        tagsRepository.setTagValues(SERVICE_ID, path, tagValues);
        verify(tagsPersistentRepository).updateOrCreate(any(ResourceReference.class), anyMap());

        val values = tagsRepository.getTagValues(SERVICE_ID, path);
        assertEquals(value.textValue(), values.get(jsonPointer).textValue());
        verifyNoMoreInteractions(tagsPersistentRepository);
    }

    @Test
    public void whenSettingTagsInRepository_shouldNotStoreIfTagsFailedToBePersisted() {
        val tagsPersistentRepository = mock(TagsPersistentRepository.class);
        doThrow(new PersistenceException()).when(tagsPersistentRepository).updateOrCreate(any(), anyMap());
        this.tagsRepository = new TagsRepository(resourceTagsCache, tagsPersistentRepository);

        val path = "/some/path";
        val existingTags = tagsRepository.getTagValues(SERVICE_ID, path);

        val tagValues = newTagBundle();
        val jsonPointer = JsonPointer.compile("/AssetTag");
        val value = stringToJsonNode("\"someValue\"");
        tagValues.put(jsonPointer, value);
        try {
            tagsRepository.setTagValues(SERVICE_ID, path, tagValues);
        } catch (PersistenceException e) {
            // do nothing
        }

        val existingTagsAfterFailedSet = tagsRepository.getTagValues(SERVICE_ID, path);
        assertEquals(existingTags.size(), existingTagsAfterFailedSet.size());
    }
}
