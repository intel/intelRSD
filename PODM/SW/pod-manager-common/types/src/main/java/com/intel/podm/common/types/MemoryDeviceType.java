package com.intel.podm.common.types;

public enum MemoryDeviceType implements EnumeratedType {
    DDR("DDR", 1),
    DDR2("DDR2", 2),
    DDR3("DDR3", 3),
    DDR4("DDR4", 4),
    DDR4_SDRRAM("DDR4_SDRAM", 5),
    DDR4E_SDRRAM("DDR4E_SDRAM", 6),
    LPDDR4_SDRAM("LPDDR4_SDRAM", 7),
    DDR3_SDRRAM("DDR3_SDRAM", 8),
    LPDDR3_SDRAM("LPDDR3_SDRAM", 9),
    DDR2_SDRRAM("DDR2_SDRAM", 10),
    DDR2_SDRRAM_FB_DIMM("DDR2_SDRAM_FB_DIMM", 11),
    DDR2_SDRRAM_FB_DIMM_PROBE("DDR2_SDRAM_FB_DIMM_PROBE", 12),
    DDR_SGRAM("DDR_SGRAM", 13),
    DDR_SDRAM("DDR_SDRAM", 14),
    ROM("ROM", 15),
    SDRAM("SDRAM", 16),
    EDO("EDO", 17),
    FAST_PAGE_MODE("FastPageMode", 18),
    PIPELINED_NIBBLE("PipelinedNibble", 19);

    private final String value;
    private final Integer priority;

    MemoryDeviceType(String value, Integer priority) {
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
