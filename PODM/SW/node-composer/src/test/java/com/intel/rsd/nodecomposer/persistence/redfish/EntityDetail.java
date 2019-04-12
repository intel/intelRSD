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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.github.javaparser.ParseException;
import com.github.javaparser.ast.CompilationUnit;
import com.github.javaparser.ast.body.ClassOrInterfaceDeclaration;
import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.visitor.VoidVisitorAdapter;
import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.PreRemove;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.lang.annotation.Annotation;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static com.github.javaparser.JavaParser.parse;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityDetail.RelationshipType.TO_MANY;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityDetail.RelationshipType.TO_ONE;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.ADDER_PREFIX;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.GETTER_PREFIX;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.SETTER_PREFIX;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.UNLINKER_PREFIX;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.extractMethodByMethodDeclaration;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.extractMethodsByEntityTypeContainedInCollectionReturnedByMethod;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getEntityTypeFromEntityFieldOrCollectionOfEntitiesField;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getEntityTypeIfMethodReturnsEntityOrCollectionOfEntities;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getExactlyOneEntityTypeFromMethodArguments;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getExpectedMethodName;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getMethodByName;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.getMethods;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.isAnnotationPresent;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.isAnyAnnotationPresent;
import static com.intel.rsd.nodecomposer.persistence.redfish.EntityRelationshipHelper.unproxy;
import static java.util.Arrays.stream;
import static java.util.Collections.singleton;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.lang3.exception.ExceptionUtils.getStackTrace;
import static org.mockito.Mockito.spy;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
class EntityDetail {
    private static final Set<Class<? extends Annotation>> TO_MANY_ANNOTATIONS = new HashSet<Class<? extends Annotation>>() {
        private static final long serialVersionUID = -2890901643767185488L;

        {
            add(javax.persistence.OneToMany.class);
            add(javax.persistence.ManyToMany.class);
        }
    };
    private static final Set<Class<? extends Annotation>> TO_ONE_ANNOTATIONS = new HashSet<Class<? extends Annotation>>() {
        private static final long serialVersionUID = 9044405964812232788L;

        {
            add(javax.persistence.OneToOne.class);
            add(javax.persistence.ManyToOne.class);
        }
    };

    private final Class<? extends Entity> entityClass;
    private final Set<EntityRelationshipDetail> entityRelationshipDetails;
    private final Set<PossibleObject<Method>> preRemoveMethods;
    private final Set<PossibleObject<Method>> allMethods;

    EntityDetail(Class<? extends Entity> entityClass) {
        this.entityClass = entityClass;
        this.entityRelationshipDetails = createEntityRelationshipDetails();
        this.preRemoveMethods = extractPreRemoveMethods();
        this.allMethods = gatherAllMethodsInOrder();
    }

    Class<? extends Entity> getEntityClass() {
        return entityClass;
    }

    Set<EntityRelationshipDetail> getEntityRelationshipDetails() {
        return entityRelationshipDetails;
    }

    List<Method> getAllMethods() {
        return allMethods.stream()
            .map(PossibleObject::get)
            .collect(toList());
    }

    List<String> validate() {
        List<String> errors = new ArrayList<>();

        entityRelationshipDetails.stream()
            .map(EntityRelationshipDetail::validate)
            .forEach(errors::addAll);

        preRemoveMethods.stream()
            .filter(PossibleObject::hasError)
            .forEach(preRemoveMethod -> errors.add(preRemoveMethod.getError()));

        allMethods.stream()
            .filter(PossibleObject::hasError)
            .forEach(preRemoveMethod -> errors.add(preRemoveMethod.getError()));

        return errors;
    }

    SpiedEntity spyWithAllRelationships() throws EntityRelationshipException {
        Entity entity = spy(entityClass);
        return new SpiedEntity(entity, spyRelationships(entity));
    }

    void invokePreRemoveMethods(Entity entity) throws EntityRelationshipException {
        for (PossibleObject<Method> preRemoveMethod : preRemoveMethods) {
            try {
                preRemoveMethod.get().invoke(entity);
            } catch (IllegalAccessException | InvocationTargetException e) {
                throw new EntityRelationshipException(entityClass.getName() + ": Could not invoke @PreRemove method: ", e);
            }
        }
    }

    Set<Method> getUnlinkerMethods(Class<? extends Entity> argumentType) {
        return entityRelationshipDetails.stream()
            .filter(entityRelationshipDetail -> entityRelationshipDetail.getTypeOfRelatedEntity().equals(argumentType))
            .map(entityRelationshipDetail -> entityRelationshipDetail.getUnlinkerMethod().get())
            .collect(toSet());
    }

    private Set<EntityRelationshipDetail> createEntityRelationshipDetails() {
        Map<Method, Class<? extends Entity>> unlinkerMethods = getMethodsWithOneArgumentOfEntityType(UNLINKER_PREFIX, Void.TYPE);
        Map<Method, Class<? extends Entity>> setterMethods = getMethodsWithOneArgumentOfEntityType(SETTER_PREFIX, Void.TYPE);
        Map<Method, Class<? extends Entity>> adderMethods = getMethodsWithOneArgumentOfEntityType(ADDER_PREFIX, Void.TYPE);
        Map<Method, Class<? extends Entity>> getterMethods = getMethodsWithNoArgumentsReturningEntityOrCollectionOfEntities(GETTER_PREFIX);

        Set<EntityRelationshipDetail> entityRelationshipDetails = new HashSet<>();
        entityRelationshipDetails.addAll(createToManyEntityRelationshipDetails(getterMethods, adderMethods, unlinkerMethods));
        entityRelationshipDetails.addAll(createToOneEntityRelationshipDetails(getterMethods, setterMethods, unlinkerMethods));

        return entityRelationshipDetails;
    }

    private Map<Method, Class<? extends Entity>> getMethodsWithOneArgumentOfEntityType(String methodPrefix, Class<?> returnType) {
        Map<Method, Class<? extends Entity>> methodsMap = new HashMap<>();
        Method[] methods = entityClass.getDeclaredMethods();

        for (Method method : methods) {
            Class<? extends Entity> entityArgumentType = getExactlyOneEntityTypeFromMethodArguments(method);
            if (method.getReturnType().equals(returnType) && method.getName().startsWith(methodPrefix) && entityArgumentType != null) {
                methodsMap.put(method, entityArgumentType);
            }
        }

        return methodsMap;
    }

    private Map<Method, Class<? extends Entity>> getMethodsWithNoArgumentsReturningEntityOrCollectionOfEntities(String methodPrefix) {
        Map<Method, Class<? extends Entity>> methodsMap = new HashMap<>();
        Method[] methods = entityClass.getDeclaredMethods();

        for (Method method : methods) {
            Class<? extends Entity> entityReturnType = getEntityTypeIfMethodReturnsEntityOrCollectionOfEntities(method);
            if (entityReturnType != null && method.getName().startsWith(methodPrefix) && method.getParameterTypes().length == 0) {
                methodsMap.put(method, entityReturnType);
            }
        }

        return methodsMap;
    }

    private Set<EntityRelationshipDetail> createToManyEntityRelationshipDetails(Map<Method, Class<? extends Entity>> getterMethods,
                                                                                Map<Method, Class<? extends Entity>> adderMethods,
                                                                                Map<Method, Class<? extends Entity>> unlinkerMethods) {
        Set<EntityRelationshipDetail> entityRelationshipDetails = new HashSet<>();

        findAnnotatedFields(TO_MANY_ANNOTATIONS).stream()
            .filter(collectionField -> !isAnnotationPresent(collectionField, IgnoreUnlinkingRelationship.class))
            .forEach(collectionField -> {
                Class<?> collectionFieldType = collectionField.getType();
                Class<? extends Entity> typeOfRelatedEntity = getEntityTypeFromEntityFieldOrCollectionOfEntitiesField(collectionField);

                PossibleObject<Method> getterMethod = extractPossibleMethod(collectionField, typeOfRelatedEntity, GETTER_PREFIX,
                    extractMethodsByEntityTypeContainedInCollectionReturnedByMethod(getMethods(getterMethods, collectionFieldType), typeOfRelatedEntity));
                PossibleObject<Method> adderMethod = extractPossibleMethod(collectionField, typeOfRelatedEntity, ADDER_PREFIX,
                    getMethods(adderMethods, Void.TYPE, typeOfRelatedEntity), true);
                PossibleObject<Method> unlinkerMethod = extractPossibleMethod(collectionField, typeOfRelatedEntity, UNLINKER_PREFIX,
                    getMethods(unlinkerMethods, Void.TYPE, typeOfRelatedEntity), true);

                entityRelationshipDetails.add(
                    new EntityRelationshipDetail(collectionField, typeOfRelatedEntity, TO_MANY, unlinkerMethod, null, adderMethod, getterMethod));
            });

        return entityRelationshipDetails;
    }

    private Set<EntityRelationshipDetail> createToOneEntityRelationshipDetails(Map<Method, Class<? extends Entity>> getterMethods,
                                                                               Map<Method, Class<? extends Entity>> setterMethods,
                                                                               Map<Method, Class<? extends Entity>> unlinkerMethods) {
        Set<EntityRelationshipDetail> entityRelationshipDetails = new HashSet<>();

        findAnnotatedFields(TO_ONE_ANNOTATIONS).stream()
            .filter(field -> !isAnnotationPresent(field, IgnoreUnlinkingRelationship.class))
            .forEach(field -> {
                Class<? extends Entity> typeOfRelatedEntity = getEntityTypeFromEntityFieldOrCollectionOfEntitiesField(field);

                PossibleObject<Method> getterMethod = extractPossibleMethod(field, typeOfRelatedEntity, GETTER_PREFIX,
                    getMethods(getterMethods, typeOfRelatedEntity));
                PossibleObject<Method> setterMethod = extractPossibleMethod(field, typeOfRelatedEntity, SETTER_PREFIX,
                    getMethods(setterMethods, Void.TYPE, typeOfRelatedEntity));
                PossibleObject<Method> unlinkerMethod = extractPossibleMethod(field, typeOfRelatedEntity, UNLINKER_PREFIX,
                    getMethods(unlinkerMethods, Void.TYPE, typeOfRelatedEntity));

                entityRelationshipDetails.add(
                    new EntityRelationshipDetail(field, typeOfRelatedEntity, TO_ONE, unlinkerMethod, setterMethod, null, getterMethod));
            });

        return entityRelationshipDetails;
    }

    private Set<Field> findAnnotatedFields(Set<Class<? extends Annotation>> annotations) {
        return stream(entityClass.getDeclaredFields())
            .filter(field -> isAnyAnnotationPresent(field, annotations))
            .collect(toSet());
    }

    private PossibleObject<Method> extractPossibleMethod(Field field, Class<? extends Entity> typeOfRelatedEntity, String prefix, Set<Method> methods) {
        return extractPossibleMethod(field, typeOfRelatedEntity, prefix, methods, false);
    }

    private PossibleObject<Method> extractPossibleMethod(Field field, Class<? extends Entity> typeOfRelatedEntity, String prefix, Set<Method> methods,
                                                         boolean depluralizeName) {
        if (methods.isEmpty()) {
            return new PossibleObject<>(null, entityClass.getName() + ": " + prefix + " method not found for field: " + field.toGenericString() + ".");
        } else if (methods.size() == 1) {
            return new PossibleObject<>(methods.iterator().next());
        }

        String error = null;
        String expectedMethodName = getExpectedMethodName(prefix, field, depluralizeName);
        Method possibleMethod = getMethodByName(methods, expectedMethodName);
        if (possibleMethod == null) {
            error = entityClass.getName() + ": Since there are more than one " + prefix + " method related to " + typeOfRelatedEntity.getSimpleName()
                + " entity, strict method naming is required. Change method name to: " + expectedMethodName + ".";
        }

        return new PossibleObject<>(possibleMethod, error);
    }

    private Set<PossibleObject<Method>> extractPreRemoveMethods() {
        Set<Method> foundMethods;

        Method[] methods = entityClass.getMethods();
        foundMethods = stream(methods)
            .filter(method -> isAnnotationPresent(method, PreRemove.class))
            .collect(toSet());

        Set<PossibleObject<Method>> possibleMethods = new HashSet<>();
        if (foundMethods.isEmpty()) {
            possibleMethods.add(new PossibleObject<>(null, entityClass.getName() + ": There is not any non abstract @PreRemove method."));
            return possibleMethods;
        }

        possibleMethods = foundMethods.stream()
            .map(PossibleObject::new)
            .collect(toSet());

        return possibleMethods;
    }

    private Set<PossibleObject<Method>> gatherAllMethodsInOrder() {
        File entityFile = new File("src/main/java/" + entityClass.getName().replace(".", "/") + ".java");

        String entityFileFullPath = entityFile.getAbsolutePath();
        if (!entityFile.isFile() || !entityFile.canRead()) {
            return singleton(
                new PossibleObject<>(null, entityClass.getName() + ": Could not read source file " + entityFileFullPath));
        }

        try (FileInputStream fileInputStream = new FileInputStream(entityFileFullPath)) {
            CompilationUnit compilationUnit = parse(fileInputStream);
            return gatherMethodsInOrder(compilationUnit);
        } catch (ParseException | IOException e) {
            return singleton(
                new PossibleObject<>(null, entityClass.getName() + ": Could not parse source file " + entityFileFullPath + ". " + getStackTrace(e)));
        }
    }

    @SuppressWarnings("checkstyle:AnonInnerLength")
    private Set<PossibleObject<Method>> gatherMethodsInOrder(CompilationUnit compilationUnit) {
        Set<PossibleObject<Method>> relationshipMethods = new LinkedHashSet<>();

        new VoidVisitorAdapter<Object>() {
            @Override
            public void visit(MethodDeclaration methodDeclaration, Object arg) {
                super.visit(methodDeclaration, arg);
                if (methodDeclaration.getParentNode() instanceof ClassOrInterfaceDeclaration
                    && !((ClassOrInterfaceDeclaration) methodDeclaration.getParentNode()).getName().equals(entityClass.getSimpleName())) {
                    //ignore private classes
                    return;
                }

                Method method = extractMethodByMethodDeclaration(entityClass, methodDeclaration);
                if (method == null) {
                    relationshipMethods.add(new PossibleObject<>(null, entityClass.getName() + ": Could not found method: "
                        + methodDeclaration.getName() + "."));
                } else {
                    relationshipMethods.add(new PossibleObject<>(method));
                }
            }
        }.visit(compilationUnit, null);

        return relationshipMethods;
    }

    private Map<EntityRelationshipDetail, Entity> spyRelationships(Entity spiedEntity) throws EntityRelationshipException {
        Map<EntityRelationshipDetail, Entity> spiedRelationships = new HashMap<>();
        for (EntityRelationshipDetail entityRelationshipDetail : entityRelationshipDetails) {
            Method method = entityRelationshipDetail.getSetterOrAdder().get();
            Entity spiedRelationship = spy(entityRelationshipDetail.getTypeOfRelatedEntity());
            try {
                method.invoke(spiedEntity, spiedRelationship);
            } catch (IllegalAccessException | InvocationTargetException e) {
                throw new EntityRelationshipException("Could not initialize " + unproxy(spiedEntity.getClass()).getSimpleName() + " -> "
                    + entityRelationshipDetail.getField().getName() + " (" + unproxy(spiedRelationship.getClass()).getSimpleName()
                    + ") relationship.", e);
            }
            spiedRelationships.put(entityRelationshipDetail, spiedRelationship);
        }

        return spiedRelationships;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        EntityDetail that = (EntityDetail) o;
        return Objects.equals(entityClass, that.entityClass);
    }

    @Override
    public int hashCode() {
        return Objects.hash(entityClass);
    }

    enum RelationshipType {
        TO_ONE, TO_MANY
    }

    static class SpiedEntity {
        private final Entity spiedEntity;
        private final Map<EntityRelationshipDetail, Entity> spiedRelationships;

        SpiedEntity(Entity spiedEntity, Map<EntityRelationshipDetail, Entity> spiedRelationships) {
            this.spiedEntity = spiedEntity;
            this.spiedRelationships = spiedRelationships;
        }

        Entity getSpiedEntity() {
            return spiedEntity;
        }

        Map<EntityRelationshipDetail, Entity> getSpiedRelationships() {
            return spiedRelationships;
        }
    }

    @SuppressWarnings({"checkstyle:ParameterNumber"})
    static class EntityRelationshipDetail {
        private final Field field;
        private final Class<? extends Entity> typeOfRelatedEntity;
        private final RelationshipType relationshipType;
        private final PossibleObject<Method> unlinkerMethod;
        private final PossibleObject<Method> setterMethod;
        private final PossibleObject<Method> adderMethod;
        private final PossibleObject<Method> getterMethod;

        EntityRelationshipDetail(Field field, Class<? extends Entity> typeOfRelatedEntity, RelationshipType relationshipType,
                                 PossibleObject<Method> unlinkerMethod, PossibleObject<Method> setterMethod, PossibleObject<Method> adderMethod,
                                 PossibleObject<Method> getterMethod) {
            this.field = field;
            this.typeOfRelatedEntity = typeOfRelatedEntity;
            this.relationshipType = relationshipType;
            this.unlinkerMethod = unlinkerMethod;
            this.setterMethod = setterMethod;
            this.adderMethod = adderMethod;
            this.getterMethod = getterMethod;
        }

        Field getField() {
            return field;
        }

        Class<? extends Entity> getTypeOfRelatedEntity() {
            return typeOfRelatedEntity;
        }

        RelationshipType getRelationshipType() {
            return relationshipType;
        }

        PossibleObject<Method> getUnlinkerMethod() {
            return unlinkerMethod;
        }

        PossibleObject<Method> getSetterMethod() {
            return setterMethod;
        }

        PossibleObject<Method> getAdderMethod() {
            return adderMethod;
        }

        PossibleObject<Method> getGetterMethod() {
            return getterMethod;
        }

        PossibleObject<Method> getSetterOrAdder() {
            if (relationshipType == TO_MANY) {
                return adderMethod;
            }

            return setterMethod;
        }

        private List<String> validate() {
            List<String> errors = new ArrayList<>();

            if (unlinkerMethod.hasError()) {
                errors.add(unlinkerMethod.getError());
            }
            if (getterMethod.hasError()) {
                errors.add(getterMethod.getError());
            }
            if (getSetterOrAdder().hasError()) {
                errors.add(getSetterOrAdder().getError());
            }

            return errors;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }

            if (o == null || getClass() != o.getClass()) {
                return false;
            }

            EntityRelationshipDetail that = (EntityRelationshipDetail) o;

            return new EqualsBuilder()
                .append(field, that.field)
                .append(typeOfRelatedEntity, that.typeOfRelatedEntity)
                .append(relationshipType, that.relationshipType)
                .append(unlinkerMethod, that.unlinkerMethod)
                .append(setterMethod, that.setterMethod)
                .append(adderMethod, that.adderMethod)
                .append(getterMethod, that.getterMethod)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(field)
                .append(typeOfRelatedEntity)
                .append(relationshipType)
                .append(unlinkerMethod)
                .append(setterMethod)
                .append(adderMethod)
                .append(getterMethod)
                .toHashCode();
        }
    }

    static class PossibleObject<T> {
        private final T object;
        private final String error;

        PossibleObject(T object) {
            this(object, null);
        }

        PossibleObject(T object, String error) {
            this.object = object;
            this.error = error;
        }

        T get() {
            return object;
        }

        boolean hasError() {
            return error != null;
        }

        String getError() {
            return error;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            PossibleObject<?> that = (PossibleObject<?>) o;
            return Objects.equals(object, that.object)
                && Objects.equals(error, that.error);
        }

        @Override
        public int hashCode() {
            return Objects.hash(object, error);
        }
    }
}
