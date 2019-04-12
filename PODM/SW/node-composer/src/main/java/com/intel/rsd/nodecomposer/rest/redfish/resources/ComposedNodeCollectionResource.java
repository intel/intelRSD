/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.resources;

import com.intel.rsd.nodecomposer.business.dto.redfish.ComposedNodeCollectionDto;
import com.intel.rsd.nodecomposer.business.redfish.services.ComposedNodeReader;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static com.intel.rsd.nodecomposer.rest.redfish.OptionsResponseBuilder.newOptionsForResourceBuilder;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping(path = COMPOSED_NODES_RESOURCE_NAME, produces = APPLICATION_JSON_VALUE)
public class ComposedNodeCollectionResource extends BaseResource {
    private final ComposedNodeReader composedNodeReader;

    @Autowired
    public ComposedNodeCollectionResource(ComposedNodeReader composedNodeReader) {
        this.composedNodeReader = composedNodeReader;
    }

    @GetMapping
    @Override
    public ResponseEntity<ComposedNodeCollectionDto> get() {
        return ok(getOrThrow(composedNodeReader::getCollection));
    }

    @Override
    protected ResponseEntity createOptionsResponse() {
        return newOptionsForResourceBuilder().build();
    }
}
