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

package com.intel.rsd.resourcemanager.layers.tagger.data;

import com.intel.rsd.resourcemanager.layers.tagger.TagBundle;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.Table;

import static com.intel.rsd.resourcemanager.layers.tagger.TagBundle.newTagBundleFrom;
import static com.intel.rsd.resourcemanager.layers.tagger.data.TaggedValuesSerializerDeserializer.asMap;

@Entity
@Table(name = "resource_tagged_values")
@NoArgsConstructor
@Data
public class ResourceTaggedValues {
    @EmbeddedId
    private ResourceReference resourceReference;

    @Column(name = "serialized_tagged_values")
    private String serializedTaggedValues;

    ResourceTaggedValues(@NonNull ResourceReference resourceReference, @NonNull String serializedTaggedValues) {
        this.resourceReference = resourceReference;
        this.serializedTaggedValues = serializedTaggedValues;
    }

    public TagBundle getTaggedValues() {
        return newTagBundleFrom(asMap(serializedTaggedValues));
    }
}
