package com.intel.podm.common.types;

public enum ErrorCorrection implements EnumeratedType {
    NO_ECC("NoECC"),
    SINGLE_BIT_ECC("SingleBitECC"),
    MULTI_BIT_ECC("MultiBitECC"),
    ADDRESS_PARITY("AddressParity");

    private final String value;

    ErrorCorrection(String value) {
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
