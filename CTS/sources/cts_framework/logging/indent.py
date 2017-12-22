def indent():
    return "    " * Indent.get_indent()


class Indent:
    _indent = 0

    @classmethod
    def get_indent(cls):
        return cls._indent

    @classmethod
    def indent_right(cls):
        old_indent = Indent._indent
        Indent._indent += 1
        return old_indent

    @classmethod
    def indent_reset(cls, new_indent=None):
        Indent._indent = 0 if new_indent is None else new_indent
