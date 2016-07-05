/*
 * Copyright (c) 2016 Intel Corporation
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
package com.intel.podm;

import com.google.common.reflect.ClassPath;
import com.intel.podm.business.entities.base.AbstractDomainObject;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectInspector;
import com.intel.podm.business.entities.base.DomainObjectLink;
import com.intel.podm.decrypter.PodmVaultDecrypter;
import com.orientechnologies.orient.core.metadata.schema.OClass;
import com.orientechnologies.orient.core.metadata.schema.OSchemaProxy;
import com.orientechnologies.orient.core.metadata.schema.OType;
import com.tinkerpop.blueprints.impls.orient.OrientGraphNoTx;
import com.tinkerpop.blueprints.impls.orient.OrientVertexType;

import java.io.IOException;
import java.util.List;
import java.util.function.Predicate;

import static com.google.common.base.Preconditions.checkArgument;
import static java.util.stream.Collectors.toList;

public final class DbSchemaCreator {

    private static final String DB_USER = "administrator";
    private static final String DB_ADDRESS = "remote:localhost/podm_db";
    private static final String VAULT_PASSWORD_PATH = "VAULT::orientdb::password::1";

    private DbSchemaCreator() {
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println("Welcome to Pod Manager's brilliant schema creation tool.");
        OrientGraphNoTx graph = new OrientGraphNoTx(DB_ADDRESS, DB_USER, getDbPassword());

        System.out.println("Creating vertices schema...");
        createVerticesSchema(graph);
        System.out.println("Creating edges schema...");
        createEdgesSchema(graph);
    }

    private static void createEdgesSchema(OrientGraphNoTx graph) throws InterruptedException {
        try (ConsoleProgressBar progressBar = new ConsoleProgressBar(DomainObjectLink.values().length)) {
            for (DomainObjectLink link : DomainObjectLink.values()) {
                if (schemaAlreadyExists(link.getLabel(), graph)) {
                    progressBar.addProgress();
                    continue;
                }

                graph.createEdgeType(link.getLabel());
                progressBar.addProgress();
            }
        }
    }

    private static void createVerticesSchema(OrientGraphNoTx graph) throws IOException, InterruptedException {
        List<Class<?>> domainObjects = getDomainObjects();
        checkArgument(domainObjects.size() != 0, "Domain objects were not found.");

        try (ConsoleProgressBar progressBar = new ConsoleProgressBar(domainObjects.size())) {
            for (Class<?> domainObjectClass : domainObjects) {
                if (schemaAlreadyExists(domainObjectClass.getSimpleName(), graph)) {
                    progressBar.addProgress();
                    continue;
                }

                OrientVertexType vertexType = graph.createVertexType(domainObjectClass.getSimpleName());
                DomainObjectInspector inspector = DomainObjectInspector.inspect((Class<? extends DomainObject>) domainObjectClass);

                inspector.getProperties().forEach((key, aClass) -> vertexType.createProperty(key, OType.getTypeByClass(aClass)));

                progressBar.addProgress();
            }
        }
    }

    private static boolean schemaAlreadyExists(String name, OrientGraphNoTx graph) {
        OSchemaProxy schema = graph.getRawGraph().getMetadata().getSchema();
        OClass orientClass = schema.getClass(name);
        return orientClass != null;
    }

    private static List<Class<?>> getDomainObjects() throws IOException {
        return ClassPath.from(ClassLoader.getSystemClassLoader()).getTopLevelClasses().stream()
                .filter(classInfo -> classInfo.getName().contains("com.intel"))
                .map(ClassPath.ClassInfo::load)
                .filter(DomainObject.class::isAssignableFrom)
                .filter(byNonAbstractEntities())
                .collect(toList());
    }

    private static Predicate<? super Class<?>> byNonAbstractEntities() {
        return aClass -> !aClass.isAnnotationPresent(AbstractDomainObject.class);
    }

    public static String getDbPassword() {
        PodmVaultDecrypter vaultDecrypter = new PodmVaultDecrypter();
        return vaultDecrypter.getPasswordUnder(VAULT_PASSWORD_PATH);
    }
}
