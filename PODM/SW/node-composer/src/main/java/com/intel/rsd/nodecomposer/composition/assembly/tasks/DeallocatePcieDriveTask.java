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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.io.Serializable;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
public class DeallocatePcieDriveTask extends NodeRemovalTask implements Serializable {
    private static final long serialVersionUID = 4790390867909436499L;

    @Autowired
    private transient GenericDao genericDao;

    private ODataId driveODataId;

    public DeallocatePcieDriveTask setDriveODataId(ODataId driveODataId) {
        this.driveODataId = driveODataId;
        return this;
    }

    @Override
    public void disassemble() {
    }

    @Override
    public void deallocate() {
        Drive drive = genericDao.find(Drive.class, driveODataId);
        drive.setAllocated(false);
    }
}
