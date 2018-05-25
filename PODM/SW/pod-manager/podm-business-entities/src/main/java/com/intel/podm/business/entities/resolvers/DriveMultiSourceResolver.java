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

package com.intel.podm.business.entities.resolvers;

import com.intel.podm.business.entities.dao.DriveDao;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.ExternalLink;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Iterator;
import java.util.Optional;

@Dependent
public class DriveMultiSourceResolver extends MultiSourceEntityResolver<Drive> {
    private final DriveDao driveDao;

    @Inject
    DriveMultiSourceResolver(DriveDao driveDao) {
        super(Drive.class);
        this.driveDao = driveDao;
    }

    @Override
    protected Optional<Drive> findPrimaryEntity(Drive complementaryDrive) {
        return driveDao.findPrimaryDrive(complementaryDrive);
    }

    @Override
    public String createMultiSourceDiscriminator(Drive drive) {
        Iterator<ExternalLink> externalLinkIterator = drive.getExternalLinks().iterator();
        if (externalLinkIterator.hasNext()) {
            return externalLinkIterator.next().getSourceUri().getPath();
        } else {
            throw new RuntimeException("Drive should have at least one ExternalLink associated with it");
        }
    }
}
