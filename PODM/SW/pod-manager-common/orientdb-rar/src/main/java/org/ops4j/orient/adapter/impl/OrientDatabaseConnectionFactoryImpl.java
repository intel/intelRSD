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

/*
 * Copyright 2013 Harald Wellmann
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.ops4j.orient.adapter.impl;

import org.ops4j.orient.adapter.api.OrientDatabaseConnection;
import org.ops4j.orient.adapter.api.OrientDatabaseConnectionFactory;
import org.ops4j.orient.adapter.api.OrientManagedConnectionFactory;

import javax.naming.NamingException;
import javax.naming.Reference;
import javax.resource.ResourceException;
import javax.resource.spi.ConnectionManager;
/**
 * @author Harald Wellmann
 *
 */
public class OrientDatabaseConnectionFactoryImpl implements OrientDatabaseConnectionFactory {
    private static final long serialVersionUID = 1L;
    private OrientManagedConnectionFactory mcf;
    private ConnectionManager cm;
    private Reference reference;
    public OrientDatabaseConnectionFactoryImpl(OrientManagedConnectionFactory mcf, ConnectionManager cm) {
        this.mcf = mcf;
        this.cm = cm;
    }
    @Override
    public void setReference(Reference reference) {
        this.reference = reference;
    }
    @Override
    public Reference getReference() throws NamingException {
        return reference;
    }
    @Override
    public OrientDatabaseConnection createConnection() throws ResourceException {
        return (OrientDatabaseConnection) cm.allocateConnection(mcf, null);
    }
}