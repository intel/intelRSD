/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.redfish.base;

import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.business.entities.redfish.ExternalService;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.ConcurrentModificationException;

import static com.intel.podm.business.entities.BusinessEntitiesTestsHelper.createEntity;

@SuppressWarnings({"checkstyle:MethodName"})
public class EntityTest {

    // Test checks if implementation of unlinkCollection method does not cause ConcurrentModificationException.
    @Test
    public void whenUnlinkCollectionMethodHasBeenChanged_shouldNotThrowConcurrentModificationException() throws Exception {
        try {
            ExternalService externalService = createEntity(ExternalService.class);
            externalService.addOwnedLink(createEntity(ExternalLink.class));
            externalService.addOwnedLink(createEntity(ExternalLink.class));

            externalService.unlinkCollection(externalService.getOwnedLinks(), externalService::unlinkOwnedLink);
        } catch (ConcurrentModificationException e) {
            Assert.fail("Method `Entity.unlinkCollection` thrown ConcurrentModificationException.");
        }
    }
}
