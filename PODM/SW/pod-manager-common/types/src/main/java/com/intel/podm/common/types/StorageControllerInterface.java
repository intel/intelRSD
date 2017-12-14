package com.intel.podm.common.types;

/**
 * Interface of StorageController asset.
 */
public enum StorageControllerInterface implements EnumeratedType {
    SATA("SATA", 1),
    SAS("SAS", 2),
    NVME("NVMe", 3);

    private final String value;
    private final Integer priority;

    StorageControllerInterface(String value, Integer priority) {
        this.value = value;
        this.priority = priority;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public Integer getPriority() {
        return priority;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
