from cts_core.commons.error import cts_error
from cts_core.validation.validation_status import ValidationStatus
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.discovery.api_resource import ApiResourceProxy

class HierarchyValidator:
    def __init__(self, discovery_container, metadata_container, metadata_constants):
        """
        :type discovery_container: DiscoveryContainer
        """
        self.discover_container = discovery_container
        self.metadata_container = metadata_container
        self.metadata_constants = metadata_constants

    def validate_path_using_contains_property(self):
        res = ValidationStatus.PASSED
        remained_to_be_visited = [r.odata_id for r in self.discover_container.physical_location_aware_resources()]
        root_resources = self.discover_container.root_resources()
        if root_resources:
            if len(root_resources) > 1:
                fmt = ', '.join(["{resources[%d].odata_id:id}" % idx for idx, _ in enumerate(root_resources)])
                cts_error("More than one root resource (with NULL ParentId). " +
                          "Expected only one. Root resources found: %s" % fmt,
                          resources=root_resources)
                res = ValidationStatus.FAILED

            for root in root_resources:
                res = ValidationStatus.join_statuses(
                    self._validate_from(root, remained_to_be_visited, parent_location=None, parent_odata_id=None),
                    res)

        else:
            if self.discover_container.physical_location_aware_resources():
                cts_error("Resources with Location in Oem property has been found, but none has NULL ParentId")
                res = ValidationStatus.FAILED

        if remained_to_be_visited:
            fmt = ', '.join(['{remained_to_be_visited[%d]:id}' % i for i, _ in enumerate(remained_to_be_visited)])
            cts_error("Not all location-aware resources visited while traversing tree using 'Link->Contains'. " +
                      "Unreachable are: %s" % fmt,
                      remained_to_be_visited=remained_to_be_visited)
            res = ValidationStatus.FAILED

        return res

    def _validate_from(self, resource, remained_to_be_visited, parent_location=None, parent_odata_id=None):
        """
        :type resource: ApiResourceProxy
        :rtype: str
        """
        res = ValidationStatus.PASSED

        if resource.odata_id in remained_to_be_visited:
            remained_to_be_visited.remove(resource.odata_id)

        location = resource.location

        if location and location.parent_id:
            if not parent_location:
                cts_error("Parent resource {parent_odata_id:id} has no location specified, " +
                          "but child resource {odata_id:id} has Location->ParentId={parent_id:ignore}",
                          parent_odata_id=parent_odata_id, odata_id=resource.odata_id, parent_id=location.parent_id)
                res = ValidationStatus.FAILED

        if parent_location:
            if location.parent_id != parent_location.id:
                cts_error("Parent resource {parent_odata_id:id} contains resource {odata_id:id}, " +
                          "but child resource has invalid Location->ParentId. " +
                          "Is: {id:ignore}. Expected: {expected:ignore}",
                          parent_odata_id=parent_odata_id,
                          odata_id=resource.odata_id,
                          id=location.parent_id,
                          expected=parent_location.id)
                res = ValidationStatus.FAILED
        if parent_odata_id and resource.contained_by:
            if parent_odata_id != resource.contained_by:
                cts_error("Resource {odata_id:id} has ContainedBy={contained_by:id}, " +
                          "but it's parent is {parent_odata_id:id}",
                          odata_id=resource.odata_id,
                          contained_by=resource.contained_by,
                          parent_odata_id=parent_odata_id)
                res = ValidationStatus.FAILED
        if res != ValidationStatus.FAILED and resource.contains:

            locations_within_scope = dict()

            for sub_resource_id in resource.contains:
                try:
                    sub_resource = self.discover_container.odataid_lookup(sub_resource_id)[0]
                    # validate that resources have unique location id within scope of their parent
                    if sub_resource.location.id in locations_within_scope:
                        fmt = ", ".join(['{locations[%d]:id}' % i for i, _
                                         in enumerate(locations_within_scope[sub_resource.location.id])])
                        cts_error("Resource {odata_id:id} has location id {location_id:ignore} " +
                                  "that conflicts with resources: %s" % fmt,
                                  odata_id=sub_resource.odata_id,
                                  location_id=sub_resource.location.id,
                                  locations=locations_within_scope[sub_resource.location.id])
                        res = ValidationStatus.FAILED
                    locations_within_scope.setdefault(sub_resource.location.id, []).append(sub_resource.odata_id)

                    res = ValidationStatus.join_statuses(res, self._validate_from(sub_resource, remained_to_be_visited,
                                                                                  location, resource.odata_id))
                except IndexError:
                    cts_error("Resource {odata_id:id} contains reference to nonexistent resource {subresource:id}",
                              odata_id=resource.odata_id, subresource=sub_resource_id)
                    res = ValidationStatus.FAILED

        return res

    def validate_all_or_none_chassis_have_location(self):
        """
        OEM may support Location RackScale extension. When it does, Location should be present in each Chassis resource
        """
        chassis_entities = [r for r in self.discover_container.iter_all(self.metadata_constants.CHASSIS)]
        chassis = [r.odata_id for r in chassis_entities]
        chassis_with_location = [r.odata_id for r in chassis_entities if r.location]
        num_chassis = len(chassis)
        num_resources_with_location = len(chassis_with_location)

        if num_chassis > 0:
            if num_resources_with_location == 0:
                cts_error("Oem/Intel_RackScale/Location not found in Chassis entities")
                return ValidationStatus.FAILED

            if num_chassis != num_resources_with_location:
                diff = [set(chassis) - set(chassis_with_location)]
                fmt = ', '.join(['{diff[%d]:id}' % i for i, _ in enumerate(diff)])
                cts_error("Some chassis have Oem/Intel_RackScale/Location some does not. See: " + fmt,
                          diff=diff)

                return ValidationStatus.FAILED

        return ValidationStatus.PASSED
