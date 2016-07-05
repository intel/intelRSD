package com.intel.podm.mappers;

import org.modelmapper.ModelMapper;
import org.modelmapper.TypeMap;
import org.modelmapper.internal.MappingContextImpl;
import org.modelmapper.spi.MatchingStrategy;
import org.modelmapper.spi.PropertyInfo;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.function.Function;

import static com.google.common.collect.Lists.newArrayList;
import static com.google.common.collect.Maps.newHashMap;
import static com.google.common.collect.Sets.newHashSet;
import static java.util.Arrays.asList;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;
import static org.modelmapper.convention.MatchingStrategies.STRICT;

/**
 * Base class for automated mappers.
 * @param <S> Source class
 * @param <T> Target Class
 */
public abstract class BrilliantMapper<S, T> implements Mapper<S, T> {
    private final Map<Class, Function> providers = newHashMap();
    private final List<TypeMap> typeMaps = newArrayList();
    private final Set<String> ignoredProperties = newHashSet();
    private final Class<S> sourceClass;
    private final Class<T> targetClass;
    private ModelMapper mapper;

    protected BrilliantMapper(Class<S> sourceClass, Class<T> targetClass) {
        this.sourceClass = sourceClass;
        this.targetClass = targetClass;
        setMatchingStrategy(STRICT);
    }

    @Override
    public void map(S source, T target) {
        typeMaps.forEach(typeMap -> typeMap.map(source, target));
        performNotAutomatedMapping(source, target);
    }

    @Override
    public Class<S> getSourceClass() {
        return sourceClass;
    }

    @Override
    public Class<T> getTargetClass() {
        return targetClass;
    }

    /**
     * Performs mapping which cannot be automated for some reasons. Try to avoid overriding this method.
     * Provide better automation mechanisms instead.
     */
    protected void performNotAutomatedMapping(S source, T target) {
    }

    protected final <X, Y> void registerProvider(Class<Y> targetClass, Function<X, Y> targetClassProvider) {
        providers.put(targetClass, targetClassProvider);
    }

    protected final void addIgnoredProperty(String ignoredProperty) {
        ignoredProperties.add(ignoredProperty);
    }

    private void configureMapper(ModelMapper modelMapper) {
        modelMapper.getConfiguration().setProvider(request -> {
            Function requestedTypeProvider = providers.get(request.getRequestedType());
            return requestedTypeProvider == null ? null : requestedTypeProvider.apply(request.getSource());
        });
        modelMapper.getConfiguration().setPropertyCondition(context -> {
            Optional<String> ignoredProperty = getDestinationPropertiesFromMappingContext((MappingContextImpl) context).stream()
                    .filter(ignoredProperties::contains)
                    .findAny();
            return !ignoredProperty.isPresent();
        });
    }

    private void setMatchingStrategy(MatchingStrategy strategy) {
        mapper = new ModelMapper();
        configureMapper(mapper);

        mapper.getConfiguration().setMatchingStrategy(strategy);

        typeMaps.clear();
        typeMaps.addAll(createTypeMaps(getSourceClass(), getTargetClass()));
    }

    /**
     * Creates type map for each interface in inheritance hierarchy.
     * @param source source Class
     * @param target target Class
     * @return List of TypeMap objects for each interface in inheritance hierarchy.
     */
    private List<TypeMap> createTypeMaps(Class source, Class target) {
        List<TypeMap> maps = newArrayList();

        Optional<TypeMap> typeMapOption = ofNullable(mapper.getTypeMap(source, target));
        TypeMap typeMap = typeMapOption.orElseGet(() -> mapper.createTypeMap(source, target));

        boolean isAnythingToMap = !typeMap.getMappings().isEmpty();
        if (isAnythingToMap) {
            maps.add(typeMap);
        }

        List<Class<?>> interfaces = asList(source.getInterfaces());
        interfaces.forEach(_interface -> maps.addAll(createTypeMaps(_interface, target)));

        return maps;
    }

    private Set<String> getDestinationPropertiesFromMappingContext(MappingContextImpl mappingContext) {
        Set<String> destinationProperties = newHashSet();
        if (mappingContext != null && mappingContext.getMapping() != null) {
            destinationProperties.addAll(
                    mappingContext.getMapping().getDestinationProperties().stream()
                            .map(PropertyInfo::getName)
                            .filter(Objects::nonNull)
                            .collect(toSet()));
        }

        return destinationProperties;
    }
}
