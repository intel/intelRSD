/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import org.hibernate.dialect.PostgreSQL94Dialect;

import static java.sql.Types.DECIMAL;
import static java.sql.Types.NUMERIC;
import static java.sql.Types.VARCHAR;
import static org.hibernate.type.StandardBasicTypes.TEXT;

public class CustomPostgreSql9Dialect extends PostgreSQL94Dialect {
    public CustomPostgreSql9Dialect() {
        registerColumnType(VARCHAR, "text");
        registerColumnType(NUMERIC, "numeric");
        registerColumnType(DECIMAL, "numeric");
        registerHibernateType(VARCHAR, TEXT.getName());
    }
}
