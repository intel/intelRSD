package com.intel.podm.common.types;

/**
 * Interface of StorageController asset.
 */
public enum StorageControllerInterface implements EnumeratedType {
    PCIE("PCIe"),
    SAS("SAS"),
    SATA("SATA");

    private String value;

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
