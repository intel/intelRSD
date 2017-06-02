package com.intel.podm.common.types;

public enum MemoryMedia implements EnumeratedType {
    DRAM("DRAM"),
    NAND("NAND"),
    PROPRIETARY("Proprietary");

    private final String value;

    MemoryMedia(String value) {
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
