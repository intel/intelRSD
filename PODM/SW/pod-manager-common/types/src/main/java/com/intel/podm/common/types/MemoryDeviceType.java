package com.intel.podm.common.types;

public enum MemoryDeviceType implements EnumeratedType {
    DDR("DDR"),
    DDR2("DDR2"),
    DDR3("DDR3"),
    DDR4("DDR4"),
    DDR4_SDRRAM("DDR4_SDRAM"),
    DDR4E_SDRRAM("DDR4E_SDRAM"),
    LPDDR4_SDRAM("LPDDR4_SDRAM"),
    DDR3_SDRRAM("DDR3_SDRAM"),
    LPDDR3_SDRAM("LPDDR3_SDRAM"),
    DDR2_SDRRAM("DDR2_SDRAM"),
    DDR2_SDRRAM_FB_DIMM("DDR2_SDRAM_FB_DIMM"),
    DDR2_SDRRAM_FB_DIMM_PROBE("DDR2_SDRAM_FB_DIMM_PROBE"),
    DDR_SGRAM("DDR_SGRAM"),
    DDR_SDRAM("DDR_SDRAM"),
    ROM("ROM"),
    SDRAM("SDRAM"),
    EDO("EDO"),
    FAST_PAGE_MODE("FastPageMode"),
    PIPELINED_NIBBLE("PipelinedNibble");

    private final String value;

    MemoryDeviceType(final String value) {
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
