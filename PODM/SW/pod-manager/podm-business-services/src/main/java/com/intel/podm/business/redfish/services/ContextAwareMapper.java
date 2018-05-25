/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.client.resources.ODataId;
import org.modelmapper.Converter;
import org.modelmapper.MappingException;
import org.modelmapper.ModelMapper;
import org.modelmapper.spi.ErrorMessage;
import org.modelmapper.spi.MappingContext;

import javax.annotation.PostConstruct;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Iterator;

import static org.apache.commons.lang3.exception.ExceptionUtils.getRootCause;

/**
 * This mapper provides functionality of basic configuration of ModelMapper,
 * extended by converter converting {@link Context} to {@link com.intel.podm.client.resources.ODataId}.
 * Mentioned converter provides translation between PodM NBI resource URI and target service's resource URI
 */
@Dependent
class ContextAwareMapper<S, T> {

    @Inject
    private Converter<Context, ODataId> contextODataIdConverter;

    private ModelMapper mapper;

    @PostConstruct
    public void init() {
        this.mapper = new ModelMapper();
        this.mapper.addConverter(contextODataIdConverter);
    }

    public T map(S source, Class<T> targetClass) throws ContextResolvingException {
        try {
            return mapper.map(source, targetClass);
        } catch (MappingException e) {
            Iterator<ErrorMessage> iterator = e.getErrorMessages().iterator();
            if (iterator.hasNext()) {
                ErrorMessage error = iterator.next();
                Throwable rootCause = getRootCause(error.getCause());
                if (rootCause instanceof ContextResolvingException) {
                    throw (ContextResolvingException) rootCause;
                }
            }
            throw e;
        }
    }

    @Dependent
    static class ContextToOdataIdConverter implements Converter<Context, ODataId> {
        @Inject
        private EntityTreeTraverser entityTreeTraverser;

        @Override
        public ODataId convert(MappingContext<Context, ODataId> context) {
            try {
                Context sourceContext = context.getSource();
                if (sourceContext == null) {
                    return null;
                }
                return new ODataId(((DiscoverableEntity) entityTreeTraverser.traverse(sourceContext)).getSourceUri());
            } catch (ContextResolvingException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
