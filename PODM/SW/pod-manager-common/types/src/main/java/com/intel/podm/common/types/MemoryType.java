package com.intel.podm.common.types;

public enum MemoryType implements EnumeratedType {
    DRAM("DRAM"),
    NVDIMM_N("NVDIMM_N"),
    NVDIMM_F("NVDIMM_F"),
    NVDIMM_P("NVDIMM_P");

    private final String value;

    MemoryType(String value) {
        this.value = value;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
