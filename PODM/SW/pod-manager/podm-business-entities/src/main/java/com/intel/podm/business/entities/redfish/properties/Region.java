package com.intel.podm.business.entities.redfish.properties;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.common.types.MemoryClassification;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class Region extends DomainObject {
    public static final DomainObjectProperty<String> REGION_ID = stringProperty("regionId");
    public static final DomainObjectProperty<MemoryClassification> MEMORY_CLASSIFICATION = enumProperty("memoryClassification", MemoryClassification.class);
    public static final DomainObjectProperty<Integer> OFFSET_MIB = integerProperty("offsetMib");
    public static final DomainObjectProperty<Integer> SIZE_MIB = integerProperty("sizeMib");

    public String getRegionId() {
        return getProperty(REGION_ID);
    }

    public void setRegionId(String regionId) {
        setProperty(REGION_ID, regionId);
    }

    public MemoryClassification getMemoryClassification() {
        return getProperty(MEMORY_CLASSIFICATION);
    }

    public void setMemoryClassification(MemoryClassification memoryClassification) {
        setProperty(MEMORY_CLASSIFICATION, memoryClassification);
    }

    public Integer getOffsetMib() {
        return getProperty(OFFSET_MIB);
    }

    public void setOffsetMib(Integer offsetMib) {
        setProperty(OFFSET_MIB, offsetMib);
    }

    public Integer getSizeMib() {
        return getProperty(SIZE_MIB);
    }

    public void setSizeMib(Integer sizeMib) {
        setProperty(SIZE_MIB, sizeMib);
    }
}
