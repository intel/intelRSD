package com.intel.podm.common.types;

/**
 * Interface of StorageController asset.
 */
//TODO: add "weight" for allocation algorithm,
// at the moment, the natural order is used (the order in which the values are declared)
public enum StorageControllerInterface implements EnumeratedType {
    SATA("SATA"),
    SAS("SAS"),
    NVME("NVMe");

    private final String value;

    StorageControllerInterface(String value) {
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
