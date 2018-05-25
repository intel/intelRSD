/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import org.testng.annotations.Test;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVE;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.common.types.Id.id;
import static java.util.Collections.emptySet;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class DriveServiceImplTest {

    @Test
    public void testEmptyDriveListInStorageServiceContext() throws ContextResolvingException {
        // prepare
        DriveServiceImpl service = new DriveServiceImpl();
        service.traverser = mock(EntityTreeTraverser.class);

        StorageService storageServiceMock = mock(StorageService.class);
        when(storageServiceMock.getDrives()).thenReturn(emptySet());
        when(service.traverser.traverse(any(Context.class))).thenReturn(storageServiceMock);
        Context context = contextOf(id(1), STORAGE_SERVICE);
        //run
        CollectionDto collection = service.getCollection(context);
        //check
        assertEquals(collection.getMembers(), emptySet());
        assertEquals(collection.getType(), DRIVE);
    }

    @Test(expectedExceptions = ContextResolvingException.class)
    public void throwExceptionForContextTypeOtherThanChassisAndStorageService() throws ContextResolvingException {
        // prepare
        DriveServiceImpl service = new DriveServiceImpl();
        service.traverser = mock(EntityTreeTraverser.class);
        // FABRIC is not CHASSIS or STORAGE_SERVICE
        Context context = contextOf(id(1), FABRIC);
        //run
        service.getCollection(context);
    }
}
