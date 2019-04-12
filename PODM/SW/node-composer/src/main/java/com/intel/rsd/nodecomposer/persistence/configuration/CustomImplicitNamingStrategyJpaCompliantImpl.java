/*
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.rsd.nodecomposer.persistence.configuration;

import org.hibernate.boot.model.naming.Identifier;
import org.hibernate.boot.model.naming.ImplicitForeignKeyNameSource;
import org.hibernate.boot.model.naming.ImplicitNamingStrategyJpaCompliantImpl;

import static java.util.Comparator.comparing;
import static org.hibernate.mapping.Constraint.hashedName;

public class CustomImplicitNamingStrategyJpaCompliantImpl extends ImplicitNamingStrategyJpaCompliantImpl {
    private static final long serialVersionUID = -5473034866222519138L;

    @Override
    public Identifier determineForeignKeyName(ImplicitForeignKeyNameSource source) {
        // Use a concatenation that guarantees uniqueness, even if identical names exist between all table and column identifiers.
        StringBuilder sb = new StringBuilder("table`" + source.getTableName() + "`");

        // Ensure a consistent ordering of columns, regardless of the order they were bound.
        source.getColumnNames().stream()
            .sorted(comparing(Identifier::getText))
            .map(column -> column.getText() == null ? "" : column.getText())
            .forEach(columnName -> sb.append("column`").append(columnName).append("`"));

        return Identifier.toIdentifier("FK_" + hashedName(sb.toString()));
    }
}
