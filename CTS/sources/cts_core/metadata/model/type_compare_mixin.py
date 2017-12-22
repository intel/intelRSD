from cts_core.metadata.comparator import Comparator


class TypeCompareMixin:
    def __init__(self):
        pass

    def compare_base_type(self, other, level):
        """
        :type other: cts_core.metadata.model.metadata_model.MetadataModel
        :type cmp: cts_core.metadata.comparator.Comparator
        :rtype: int
        """
        cmp = Comparator(level)
        if self.base_type != other.base_type:
            fmt = "Base type: {base}"
            cmp.message_left(fmt.format(base=self.base_type))
            cmp.message_right(fmt.format(base=other.base_type))
            cmp.set_not_equal()

        return cmp

    def compare_properties(self, other, level):
        """
        :type other: cts_core.metadata.model.metadata_model.MetadataModel
        :rtype: int
        """
        cmp = Comparator(level)

        properties = set(self.properties.keys())
        properties_other = set(other.properties.keys())
        all = sorted(list(properties.union(properties_other)))
        both = properties.intersection(properties_other)
        fmt = "Property: {name}"

        for property in all:
            if property in both:
                cmp.merge_result(self.properties[property].compare(other.properties[property], level))
            elif property in properties:
                cmp.set_not_equal()
                cmp.message_left(fmt.format(name=property))
                cmp.message_right('?')
            else:
                cmp.set_not_equal()
                cmp.message_left('?')
                cmp.message_right(fmt.format(name=property))
        return cmp

    def compare_annotations(self, other, level):
        """
        :type other: cts_core.metadata.model.metadata_model.MetadataModel
        :rtype: int
        """
        cmp = Comparator(level)

        annotations = set(self.annotations.keys())
        annotations_other = set(other.annotations.keys())
        all = sorted(list(annotations.union(annotations_other)))
        both = annotations.intersection(annotations_other)

        fmt = "Annotation: {name}"
        for annotation in all:
            if annotation in both:
                cmp.merge_result(self.annotations[annotation].compare(other.annotations[annotation], level))
            elif annotation in annotations:
                cmp.set_not_equal()
                cmp.message_left(fmt.format(name=annotation))
                cmp.message_right('?')
            else:
                cmp.set_not_equal()
                cmp.message_left('?')
                cmp.message_right(fmt.format(name=annotation))

        return cmp
