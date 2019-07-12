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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ComposedNodeUpdateHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.rest.redfish.json.actions.ComposedNodePartialRepresentation;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class ComposedNodeUpdateService {
    private final ComposedNodeUpdateHelper composedNodeUpdateHelper;

    @Autowired
    public ComposedNodeUpdateService(ComposedNodeUpdateHelper composedNodeUpdateHelper) {
        this.composedNodeUpdateHelper = composedNodeUpdateHelper;
    }

    public void perform(ODataId composedNodeODataId, ComposedNodePartialRepresentation representation) throws BusinessApiException {
        composedNodeUpdateHelper.updateComposedNode(composedNodeODataId, representation);
    }
}
