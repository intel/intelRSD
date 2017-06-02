package com.intel.podm.business.entities.converters;

import javax.persistence.AttributeConverter;
import javax.persistence.Converter;
import java.sql.Timestamp;
import java.time.LocalDateTime;

@Converter(autoApply = true)
public class LocalDateTimeConverter implements AttributeConverter<LocalDateTime, Timestamp> {
    @Override
    public Timestamp convertToDatabaseColumn(LocalDateTime value) {
        return value == null ? null : Timestamp.valueOf(value);
    }

    @Override
    public LocalDateTime convertToEntityAttribute(Timestamp value) {
        return value == null ? null : value.toLocalDateTime();
    }
}
