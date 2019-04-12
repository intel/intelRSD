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

import com.github.javaparser.ast.body.MethodDeclaration;
import com.github.javaparser.ast.type.ClassOrInterfaceType;
import com.github.javaparser.ast.type.ReferenceType;
import com.intel.rsd.nodecomposer.persistence.redfish.EntityDetail.EntityRelationshipDetail;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.modeshape.common.text.Inflector;

import javax.persistence.Enumerated;
import java.lang.annotation.Annotation;
import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.EntityDetailsGatherer.getEntityDetails;
import static java.util.Arrays.stream;
import static java.util.Collections.singleton;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.EnumType.STRING;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
final class EntityRelationshipHelper {
    static final String GETTER_PREFIX = "get";
    static final String SETTER_PREFIX = "set";
    static final String ADDER_PREFIX = "add";
    static final String UNLINKER_PREFIX = "unlink";

    private static final Inflector INFLECTOR = Inflector.getInstance();

    static {
        INFLECTOR.addIrregular("Chassis", "Chassis");
        INFLECTOR.addIrregular("Drive", "Drives");
    }

    private EntityRelationshipHelper() {
    }

    @SuppressWarnings("unchecked")
    static <T> Class<T> unproxy(Class<? extends T> proxiedClass) {
        String name = proxiedClass.getSimpleName();

        if (name.contains("$")) {
            return (Class<T>) proxiedClass.getSuperclass();
        }

        return (Class<T>) proxiedClass;
    }

    static EntityDetail getEntityDetailByEntityType(Class<? extends Entity> clazz) throws EntityRelationshipException {
        for (EntityDetail entityDetail : getEntityDetails()) {
            if (entityDetail.getEntityClass().equals(clazz)) {
                return entityDetail;
            }
        }

        throw new EntityRelationshipException("Could not find EntityDetail for class: " + clazz.getName() + ".");
    }

    static Set<Method> getMethods(Map<Method, Class<? extends Entity>> methods, Class<?> returnType, Class<?>... parameterTypes) {
        return methods.keySet().stream()
            .filter(method -> method.getReturnType().equals(returnType) && Arrays.equals(method.getParameterTypes(), parameterTypes))
            .collect(toSet());
    }

    static Method getMethodByName(Set<Method> methods, String expectedMethodName) {
        return methods.stream()
            .filter(method -> method.getName().equals(expectedMethodName))
            .findFirst()
            .orElse(null);
    }

    static Entity invokeGetMethodAndReturnEntityContained(Entity entity, EntityRelationshipDetail entityRelationshipDetail)
        throws EntityRelationshipException {
        Method getterMethod = entityRelationshipDetail.getGetterMethod().get();
        Object returnedObject = invokeGetMethod(entity, getterMethod);

        if (returnedObject instanceof Entity) {
            return (Entity) returnedObject;
        } else if (returnedObject instanceof Collection<?>) {
            @SuppressWarnings("unchecked")
            Collection<? extends Entity> entities = (Collection<? extends Entity>) returnedObject;
            if (entities.size() == 1) {
                return entities.iterator().next();
            }

            throw new EntityRelationshipException(getterMethod.getName() + " in " + unproxy(entity.getClass()).getName() + " returned collection with "
                + entities.size() + " entities instead of collection with one entity.");
        }

        throw new EntityRelationshipException(getterMethod.getName() + " for " + entity.getClass().getName() + " returned wrong object: " + returnedObject);
    }

    static void validateIfGetMethodReturnsNullOrEmptyCollection(Entity entity, EntityRelationshipDetail entityRelationshipDetail)
        throws EntityRelationshipException {
        Method getterMethod = entityRelationshipDetail.getGetterMethod().get();
        Object returnedObject = invokeGetMethod(entity, getterMethod);

        if (returnedObject instanceof Collection<?>) {
            if (!((Collection<?>) returnedObject).isEmpty()) {
                throw new EntityRelationshipException(getterMethod.getName() + " in " + unproxy(entity.getClass()).getName()
                    + " returned collection with " + ((Collection<?>) returnedObject).size() + " object(s) instead of empty collection.");
            }
        } else if (returnedObject != null) {
            throw new EntityRelationshipException(getterMethod.getName() + " for " + entity.getClass().getName() + " returned: " + returnedObject
                + " instead of null.");
        }
    }

    @SuppressWarnings("unchecked")
    static Class<? extends Entity> getExactlyOneEntityTypeFromMethodArguments(Method method) {
        if (method.getParameterTypes().length == 1 && Entity.class.isAssignableFrom(method.getParameterTypes()[0])) {
            return (Class<? extends Entity>) method.getParameterTypes()[0];
        }

        return null;
    }

    static String getExpectedMethodName(String prefix, Field field, boolean depluralize) {
        String fieldName = field.getName();

        if (depluralize) {
            fieldName = INFLECTOR.singularize(fieldName);
        }

        return prefix + fieldName.substring(0, 1).toUpperCase() + fieldName.substring(1);
    }

    static boolean isAnnotationPresent(AccessibleObject accessibleObject, Class<? extends Annotation> annotation) {
        return isAnyAnnotationPresent(accessibleObject, singleton(annotation));
    }

    static boolean isEnumeratedAnnotationWithStringValuePresent(Field field) {
        Enumerated enumerated = field.getAnnotation(Enumerated.class);
        return enumerated != null && enumerated.value() == STRING;
    }

    static Set<Method> extractMethodsByEntityTypeContainedInCollectionReturnedByMethod(Set<Method> methods, Class<? extends Entity> entityType) {
        return methods.stream()
            .filter(method -> getEntityTypeIfMethodReturnsEntityOrCollectionOfEntities(method).equals(entityType))
            .collect(toSet());
    }

    static Class<? extends Entity> getEntityTypeIfMethodReturnsEntityOrCollectionOfEntities(Method method) {
        return getEntityType(method.getReturnType(), method.getGenericReturnType());
    }

    static Class<? extends Entity> getEntityTypeFromEntityFieldOrCollectionOfEntitiesField(Field field) {
        return getEntityType(field.getType(), field.getGenericType());
    }

    static Class<? extends Enum<?>> getEnumTypeFromEnumFieldOrCollectionOfEnumsField(Field field) {
        return getEnumType(field.getType(), field.getGenericType());
    }

    static Method extractMethodByMethodDeclaration(Class<? extends Entity> entityClass, MethodDeclaration methodDeclaration) {
        for (Method method : entityClass.getDeclaredMethods()) {
            if (method.getName().equals(methodDeclaration.getName())) {
                List<String> argumentTypesFromClass = extractArgumentTypesAsStringsFromMethod(method);
                List<String> argumentTypesFromSource = extractArgumentTypesAsStringsFromMethodDeclaration(methodDeclaration);

                String returnTypeFromClass = method.getReturnType().getSimpleName();
                String returnTypeFromSource = methodDeclaration.getType().toStringWithoutComments();
                if (methodDeclaration.getType() instanceof ReferenceType) {
                    returnTypeFromSource = ((ClassOrInterfaceType) ((ReferenceType) methodDeclaration.getType()).getType()).getName();
                }

                if (argumentTypesFromClass.equals(argumentTypesFromSource) && returnTypeFromClass.equals(returnTypeFromSource)) {
                    return method;
                }
            }
        }

        return null;
    }

    static boolean isAnyAnnotationPresent(AccessibleObject accessibleObject, Set<Class<? extends Annotation>> annotations) {
        return annotations.stream()
            .anyMatch(accessibleObject::isAnnotationPresent);
    }

    @SuppressWarnings("unchecked")
    private static Class<? extends Entity> getEntityType(Class<?> type, Type genericType) {
        if (Entity.class.isAssignableFrom(type)) {
            return (Class<? extends Entity>) type;
        } else if (genericType instanceof ParameterizedType) {
            Type actualType = ((ParameterizedType) genericType).getActualTypeArguments()[0];
            if (actualType instanceof Class && Entity.class.isAssignableFrom((Class<?>) actualType)) {
                return (Class<? extends Entity>) actualType;
            }
        }

        return null;
    }

    @SuppressWarnings("unchecked")
    private static Class<? extends Enum<?>> getEnumType(Class<?> type, Type genericType) {
        if (Enum.class.isAssignableFrom(type)) {
            return (Class<? extends Enum<?>>) type;
        } else if (genericType instanceof ParameterizedType) {
            Type actualType = ((ParameterizedType) genericType).getActualTypeArguments()[0];
            if (actualType instanceof Class && Enum.class.isAssignableFrom((Class<?>) actualType)) {
                return (Class<? extends Enum<?>>) actualType;
            }
        }

        return null;
    }

    private static Object invokeGetMethod(Entity entity, Method getterMethod) throws EntityRelationshipException {
        try {
            return getterMethod.invoke(entity);
        } catch (IllegalAccessException | InvocationTargetException e) {
            throw new EntityRelationshipException("Could not invoke method " + getterMethod.getName() + " for " + entity.getClass().getName() + ".");
        }
    }

    private static List<String> extractArgumentTypesAsStringsFromMethod(Method method) {
        return stream(method.getParameters())
            .map(java.lang.reflect.Parameter::getType)
            .map(Class::getSimpleName)
            .collect(toList());
    }

    private static List<String> extractArgumentTypesAsStringsFromMethodDeclaration(MethodDeclaration methodDeclaration) {
        return methodDeclaration.getParameters().stream()
            .map(parameter -> {
                String className = parameter.getType().toStringWithoutComments();
                if (parameter.getType() instanceof ReferenceType) {
                    className = ((ClassOrInterfaceType) ((ReferenceType) parameter.getType()).getType()).getName();
                }

                return parameter.isVarArgs() ? className + "[]" : className;
            })
            .collect(toList());
    }
}
