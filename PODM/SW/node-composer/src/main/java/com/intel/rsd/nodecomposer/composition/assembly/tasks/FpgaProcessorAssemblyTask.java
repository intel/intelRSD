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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.attach.ProcessorAttacher;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@Component
@SpringAware
@Scope(SCOPE_PROTOTYPE)
public class FpgaProcessorAssemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = -9025938445493545805L;

    @Autowired
    private transient GenericDao genericDao;
    @Autowired
    private transient ProcessorAttacher processorAttacher;
    @Setter
    private ODataId fpgaProcessorUri;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() throws RuntimeException {
        ComposedNode composedNode = getComposedNode();
        Processor processor = genericDao.find(Processor.class, fpgaProcessorUri);
        try {
            processorAttacher.attach(composedNode, processor);
        } catch (BusinessApiException e) {
            log.error("Processor assembly task failed to attach processor {} to composed node {}, details: {}",
                processor.getUri(), composedNodeODataId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    public ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, composedNodeODataId);
    }
}
