/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.services.context;

import com.intel.podm.business.services.redfish.odataid.ContextTypeToResourceNameMapper;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Pair;

import java.net.URI;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Queue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.google.common.collect.Lists.reverse;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.Pair.pairOf;
import static com.intel.podm.common.utils.Contracts.requires;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Arrays.stream;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toCollection;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public final class UriToContextConverter {
    static final Map<ContextType, List<UriPatternDetail>> CONTEXT_TYPE_TO_PATTERN;

    private static final ContextTypeToResourceNameMapper CONTEXT_TYPE_TO_RESOURCE_NAME_MAPPER = new ContextTypeToResourceNameMapper();
    private static final ParamNameContextTypeMapper PARAM_NAME_CONTEXT_TYPE_MAPPER = new ParamNameContextTypeMapper();
    private static final ContextTypeToEmbeddableMapper CONTEXT_TYPE_TO_EMBEDDABLE_MAPPER = new ContextTypeToEmbeddableMapper();
    private static final ContextTypeToSingletonResourceMapper CONTEXT_TYPE_TO_SINGLETON_RESOURCE_MAPPER = new ContextTypeToSingletonResourceMapper();

    static {
        CONTEXT_TYPE_TO_PATTERN = initializeContextTypeToPatternMap();
    }

    private UriToContextConverter() {
    }

    public static Context getContextFromUri(URI uri) {
        return getContextFromUriWithPossibleParent(uri, null, ContextType.values());
    }

    public static Context getContextFromUri(URI uri, ContextType... possibleContextTypes) {
        return getContextFromUriWithPossibleParent(uri, null, possibleContextTypes);
    }

    public static Context getContextFromUriWithPossibleParent(URI uri, ContextType possibleParent, ContextType... possibleContextTypes) {
        requiresNonNull(uri, "uri");
        requiresNonNull(possibleContextTypes, "possibleContextTypes");
        requires(possibleContextTypes.length > 0, "List of possible context types cannot be empty");

        return stream(possibleContextTypes)
            .map(contextType -> getContextFromUriWithPossibleParentAndContextType(uri, possibleParent, contextType))
            .filter(Objects::nonNull)
            .findAny()
            .orElseThrow(() -> new UriConversionException(
                format("Cannot parse URI [%s] to any type from %s", uri, Arrays.toString(possibleContextTypes)), uri, possibleContextTypes)
            );
    }

    private static Map<ContextType, List<UriPatternDetail>> initializeContextTypeToPatternMap() {
        Map<ContextType, List<UriPatternDetail>> contextTypeToPattern = new HashMap<>();
        for (ContextType contextType : ContextType.values()) {
            List<List<ContextType>> allPossiblePaths = new ArrayList<>();
            findAllPossiblePathsForContextType(contextType, new ArrayList<>(), allPossiblePaths);

            List<UriPatternDetail> uriPatternDetails = allPossiblePaths.stream()
                .map(UriToContextConverter::createUriPatternDetail)
                .collect(toList());

            if (!uriPatternDetails.isEmpty()) {
                contextTypeToPattern.put(contextType, uriPatternDetails);
            }
        }
        return contextTypeToPattern;
    }

    private static void findAllPossiblePathsForContextType(ContextType contextType, List<ContextType> currentPath, List<List<ContextType>> allPaths) {
        currentPath.add(contextType);

        if (!contextType.getPossibleParents().isEmpty()) {
            for (ContextType parentContextType : contextType.getPossibleParents()) {
                findAllPossiblePathsForContextType(parentContextType, new ArrayList<>(currentPath), allPaths);
            }
        } else {
            allPaths.add(reverse(currentPath));
        }
    }

    private static UriPatternDetail createUriPatternDetail(List<ContextType> path) {
        //example patterns:
        // /redfish/v1/Systems/(?<systemId>[a-zA-Z_\-\+0-9]+)/Processors/(?<processorId>[a-zA-Z_\-\+0-9]+)
        // /redfish/v1/Chassis/(?<chassisId>[a-zA-Z_\-\+0-9]+)/Thermal#/Fans/(?<thermalFanId>[a-zA-Z_\-\+0-9]+)

        StringBuilder sb = new StringBuilder("/redfish/v1");
        List<Pair<String, ContextType>> parameterNameContextTypePairs = new ArrayList<>(path.size());
        for (ContextType currentContextType : path) {
            String parameterName = getParameterName(currentContextType);
            String resourceName = CONTEXT_TYPE_TO_RESOURCE_NAME_MAPPER.get(currentContextType);

            if (CONTEXT_TYPE_TO_EMBEDDABLE_MAPPER.isEmbeddable(currentContextType)) {
                sb.append("#");
            }

            sb.append("/");
            sb.append(resourceName)
                .append(parameterName.length() == 0 ? parameterName : "/(?<" + parameterName + ">[a-zA-Z_\\-\\+0-9]+)");

            parameterNameContextTypePairs.add(pairOf(parameterName, currentContextType));
        }

        return new UriPatternDetail(Pattern.compile(sb.toString()), parameterNameContextTypePairs);
    }

    private static String getParameterName(ContextType contextType) {
        if (CONTEXT_TYPE_TO_SINGLETON_RESOURCE_MAPPER.isSingletonResource(contextType)) {
            return "";
        }

        return PARAM_NAME_CONTEXT_TYPE_MAPPER.get(contextType).replaceAll("\\{", "").replaceAll("}", "");
    }

    private static Context getContextFromUriWithPossibleParentAndContextType(URI uri, ContextType possibleParent, ContextType contextType) {
        List<UriPatternDetail> uriPatternDetails = CONTEXT_TYPE_TO_PATTERN.getOrDefault(contextType, emptyList());
        for (UriPatternDetail uriPatternDetail : uriPatternDetails) {
            Matcher matcher = uriPatternDetail.getPattern().matcher(uri.toString());
            if (!matcher.matches() || !uriPatternDetail.doesContextTypeExistInContextTypePath(possibleParent)) {
                continue;
            }

            Queue<Pair<Id, ContextType>> idContextTypePairs = uriPatternDetail.getParameterNameContextTypePairs().stream()
                .map(parameterNameContextTypePair -> getIdContextTypePair(parameterNameContextTypePair, matcher))
                .collect(toCollection(ArrayDeque::new));

            return createContextFromIdsAndContextTypes(idContextTypePairs);
        }

        return null;
    }

    private static Pair<Id, ContextType> getIdContextTypePair(Pair<String, ContextType> parameterNameContextTypePair, Matcher matcher) {
        ContextType contextType = parameterNameContextTypePair.second();
        if (parameterNameContextTypePair.first().length() == 0) {
            return pairOf(id(""), contextType);
        }
        return pairOf(id(matcher.group(parameterNameContextTypePair.first())), contextType);
    }

    private static Context createContextFromIdsAndContextTypes(Queue<Pair<Id, ContextType>> idContextTypePairs) {
        Pair<Id, ContextType> firstIdContextTypePair = idContextTypePairs.poll();
        Context context = contextOf(firstIdContextTypePair.first(), firstIdContextTypePair.second());

        while (!idContextTypePairs.isEmpty()) {
            Pair<Id, ContextType> idContextTypePair = idContextTypePairs.poll();
            context = context.child(idContextTypePair.first(), idContextTypePair.second());
        }

        return context;
    }

    static final class UriPatternDetail {
        private final Pattern pattern;
        private final List<Pair<String, ContextType>> parameterNameContextTypePairs = new ArrayList<>();

        UriPatternDetail(Pattern pattern, List<Pair<String, ContextType>> parameterNameContextTypePairs) {
            this.pattern = pattern;
            this.parameterNameContextTypePairs.addAll(parameterNameContextTypePairs);
        }

        public Pattern getPattern() {
            return pattern;
        }

        public List<Pair<String, ContextType>> getParameterNameContextTypePairs() {
            return parameterNameContextTypePairs;
        }

        public boolean doesContextTypeExistInContextTypePath(ContextType possibleParent) {
            return possibleParent == null
                ||
                parameterNameContextTypePairs.stream()
                    .map(Pair::second)
                    .anyMatch(contextType -> contextType.equals(possibleParent));
        }
    }
}
