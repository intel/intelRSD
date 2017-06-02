package com.intel.podm.common.types;

public enum OperatingMemoryMode implements EnumeratedType {
    VOLATILE("Volatile"),
    PMEM("PMEM"),
    BLOCK("Block");

    private final String value;

    OperatingMemoryMode(String value) {
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
