/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.rest.odataid;

import com.intel.podm.redfish.resources.ServiceRootResource;
import org.testng.annotations.Test;

import javax.ws.rs.Path;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.types.redfish.ODataServices.ODATA_ROOT_SERVICES;
import static com.intel.podm.common.types.redfish.ResourceNames.ALL_RESOURCE_NAMES;
import static java.lang.String.format;
import static java.util.Arrays.stream;
import static java.util.stream.Collectors.toSet;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class ODataServiceDocumentTest {

    @Test
    public void checkWhetherAllResourcesPresentInRootServiceArePresentInODataServiceDocument() {
        Set<String> services = stream(ServiceRootResource.class.getMethods())
            .map(method -> method.getAnnotation(Path.class))
            .filter(Objects::nonNull)
            .map(Path::value)
            .filter(ALL_RESOURCE_NAMES::contains)
            .collect(toSet());

        assertTrue(services.size() > 0, "At least one service should be present at ServiceRoot.");
        services.forEach(service ->
            assertTrue(ODATA_ROOT_SERVICES.contains(service), format("%s should be present in ODATA_ROOT_SERVICES map", service))
        );
        assertEquals(ODATA_ROOT_SERVICES.size(), services.size());
    }
}
