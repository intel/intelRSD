/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.queryparameter;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.expression.Expression;
import org.springframework.expression.ExpressionParser;
import org.springframework.expression.ParseException;
import org.springframework.expression.spel.SpelEvaluationException;
import org.springframework.expression.spel.SpelMessage;
import org.springframework.expression.spel.standard.SpelExpressionParser;
import org.springframework.expression.spel.support.StandardEvaluationContext;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

import static com.google.common.collect.Sets.immutableEnumSet;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterType.FILTER;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.RANDOM_STRING;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_MEMBERS_COUNT;
import static com.intel.rsd.resourcemanager.layers.queryparameter.QueryParameterConstants.REDFISH_ODATA_ID;
import static java.util.stream.Collectors.toSet;
import static org.springframework.expression.spel.SpelMessage.NOT_COMPARABLE;
import static org.springframework.expression.spel.SpelMessage.PROPERTY_OR_FIELD_NOT_READABLE;
import static org.springframework.expression.spel.SpelMessage.PROPERTY_OR_FIELD_NOT_READABLE_ON_NULL;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.NOT_IMPLEMENTED;
import static org.springframework.http.ResponseEntity.status;

@Slf4j
@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ResourceCollectionFilter {
    private final JsonStringPropertyAccessor jsonStringPropertyAccessor;
    private final NonNullTypeComparator nonNullTypeComparator;
    private final RedfishSpelPropertyConverter converter;
    private final Set<SpelMessage> ignoredExceptions = immutableEnumSet(PROPERTY_OR_FIELD_NOT_READABLE, PROPERTY_OR_FIELD_NOT_READABLE_ON_NULL, NOT_COMPARABLE);
    private ExpressionParser parser = new SpelExpressionParser();

    public ResourceCollectionFilter(JsonStringPropertyAccessor jsonStringPropertyAccessor, NonNullTypeComparator nonNullTypeComparator,
                                    RedfishSpelPropertyConverter converter) {
        this.jsonStringPropertyAccessor = jsonStringPropertyAccessor;
        this.nonNullTypeComparator = nonNullTypeComparator;
        this.converter = converter;
    }

    public Response filter(Response response, Map<QueryParameterType, String> requestParameters, Collection<Response> resources) {
        Set<JsonNode> filteredResources;
        try {
            val expression = requestParameters.get(FILTER);
            filteredResources = filterResourcesByExpression(resources, expression);
        } catch (ParseException e) {
            return new Response(status(NOT_IMPLEMENTED).build());
        } catch (SpelEvaluationException e) {
            return new Response(status(BAD_REQUEST).build());
        }

        return new Response(buildResponseBody(response.getBody(), filteredResources), response.getHttpHeaders(), response.getHttpStatus());
    }

    private JsonNode buildResponseBody(JsonNode body, Set<JsonNode> filteredResources) {
        val collectionMembers = JsonNodeFactory.instance.arrayNode();
        collectionMembers.addAll(filteredResources);

        val nodeAmount = JsonNodeFactory.instance.numberNode(collectionMembers.size());
        ((ObjectNode) body).replace(REDFISH_MEMBERS, collectionMembers);
        ((ObjectNode) body).replace(REDFISH_MEMBERS_COUNT, nodeAmount);

        return body;
    }

    private Set<JsonNode> filterResourcesByExpression(Collection<Response> resources, String expression) {
        return resources.stream()
            .map(Response::getBody)
            .filter(jsonNode -> evaluateExpression(jsonNode, expression))
            .map(jsonNode -> jsonNode.get(REDFISH_ODATA_ID))
            .map(jsonNode -> JsonNodeFactory.instance.objectNode().set(REDFISH_ODATA_ID, jsonNode))
            .collect(toSet());
    }

    private boolean evaluateExpression(JsonNode jsonNode, String expression) {
        StandardEvaluationContext context = new StandardEvaluationContext(jsonNode);
        context.addPropertyAccessor(jsonStringPropertyAccessor);
        context.setTypeComparator(nonNullTypeComparator);
        Expression exp = parser.parseExpression(replaceSpelSpecialCharacters(expression));
        try {
            return exp.getValue(context, Boolean.class);
        } catch (SpelEvaluationException e) {
            if (ignoredExceptions.contains(e.getMessageCode())) {
                return false;
            }
            throw e;
        }
    }

    private String replaceSpelSpecialCharacters(String expression) {
        return converter.convertRedfishAllowableValuesTo(expression, RANDOM_STRING)
            .replace("/", ".");
    }
}
