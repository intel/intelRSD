"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
import sys

from cts_core.commons.error import cts_error
from cts_core.metadata.diff.metadata_loader import LOADER_CHAIN


class MetadataComparator:
    def __init__(self, metadata_list, qualifiers=None):
        self.metadata_list = metadata_list
        self.qualifiers = qualifiers if qualifiers else []

    def _load_metadata(self, metadata_ref):
        for loader in LOADER_CHAIN:
            metadata_container = loader().load(metadata_ref, self.qualifiers)
            if metadata_container is not None:
                return metadata_container

        msg = "Unable to load metadata: {ref}".format(ref=metadata_ref)
        cts_error(msg)
        sys.stderr.write(msg + "\nExiting...")
        exit(1)

    def run(self):
        print "Comparing {b} to {a}".format(b=self.metadata_list[1], a=self.metadata_list[0])
        print "Qualifiers: {qualifiers}\n".format(qualifiers=", ".join(self.qualifiers))

        metadata_containers = filter(None, [self._load_metadata(metadata_ref) for metadata_ref in self.metadata_list])
        if len(metadata_containers) != 2:
            return None

        cmp = metadata_containers[0].compare(metadata_containers[1], with_progress_bar=True)
        return cmp
