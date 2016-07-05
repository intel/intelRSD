/*
 * Copyright (c) 2015 Intel Corporation
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


package org.ops4j.orient.adapter.impl;

import com.orientechnologies.orient.core.OConstants;

import javax.resource.ResourceException;
import javax.resource.spi.ManagedConnectionMetaData;
/**
 * @author Harald Wellmann
 *
 */
public class OrientManagedConnectionMetaData implements ManagedConnectionMetaData {
    @Override
    public String getEISProductName() throws ResourceException {
        return "OrientDB";
    }
    @Override
    public String getEISProductVersion() throws ResourceException {
        return OConstants.getVersion();
    }
    @Override
    public int getMaxConnections() throws ResourceException {
        return 0;
    }
    @Override
    public String getUserName() throws ResourceException {
        return "admin";
    }
}