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

import com.intel.podm.repositories.orientdb.graph.annotation.PodGraph;
import com.tinkerpop.blueprints.impls.orient.OrientExtendedGraph;
import org.ops4j.orient.adapter.api.OrientDatabaseConnection;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Produces;
import javax.inject.Inject;

@ApplicationScoped
public class ExtendedGraphProducer {

    @Inject
    @PodGraph(transactional = false)
    OrientDatabaseConnection odc;

    @Produces
    public OrientExtendedGraph getOrientExtendedGraph() {
        return odc.ograph();
    }
}
