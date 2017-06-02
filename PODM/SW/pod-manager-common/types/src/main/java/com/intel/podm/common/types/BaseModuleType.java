package com.intel.podm.common.types;

public enum BaseModuleType implements EnumeratedType {
    RDIMM("RDIMM"),
    UDIMM("UDIMM"),
    SO_DIMM("SO_DIMM"),
    LRDIMM("LRDIMM"),
    MINI_RDIMM("Mini_RDIMM"),
    MINI_UDIMM("Mini_UDIMM"),
    RDIMM_SO_72B("SO_RDIMM_72b"),
    UDIMM_SO_72B("SO_UDIMM_72b"),
    DIMM_SO_16B("SO_DIMM_16b"),
    DIMM_SO_32B("SO_DIMM_32b");

    private final String value;

    BaseModuleType(String value) {
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
