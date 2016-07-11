"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from sqlalchemy import Column, Integer, String, DateTime

import datetime

from cts_framework.db.model.base import Base


class TestRunModel(Base):
    __tablename__ = "test_runs"

    test_run_id = Column("test_run_id", Integer, primary_key=True)
    test_run_comment = Column("test_run_comment ", String)
    test_run_datetime = Column("test_run_datetime", DateTime, default=datetime.datetime.now,
                               onupdate=datetime.datetime.now)
