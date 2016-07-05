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

package com.intel.podm.repositories.orientdb.graph;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.repositories.orientdb.graph.annotation.PodGraph;
import org.ops4j.orient.adapter.api.OrientDatabaseConnection;
import org.ops4j.orient.adapter.api.OrientDatabaseConnectionFactory;

import javax.annotation.Resource;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.context.RequestScoped;
import javax.enterprise.inject.Disposes;
import javax.enterprise.inject.Produces;
import javax.inject.Inject;
import javax.resource.ResourceException;
import javax.transaction.TransactionScoped;


@ApplicationScoped
public class GraphConnectionProducer {

    @Inject
    private Logger logger;

    @Resource(lookup = "orient/ConnectionFactory")
    private OrientDatabaseConnectionFactory odc;

    @Produces @PodGraph(transactional = true)
    @TransactionScoped
    public OrientDatabaseConnection createTransactional() {
        try {
            return odc.createConnection();
        } catch (ResourceException e) {
            throw new RuntimeException(e);
        }
    }

    @Produces @PodGraph(transactional = false)
    @RequestScoped
    public OrientDatabaseConnection create() {
        try {
            return odc.createConnection();
        } catch (ResourceException e) {
            throw new RuntimeException(e);
        }
    }

    public void close(@Disposes @PodGraph(transactional = false) OrientDatabaseConnection conn) {
        conn.close();
    }
}
