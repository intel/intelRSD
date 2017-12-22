import os, re, subprocess
from os import path

import unittest

import cts_core
import cts_framework


class TestImports(unittest.TestCase):

    def find_submodules(self, modules):
        """
        Find all the files ending with .py
        """
        for module in modules:
            modulepath = module.__path__[0]
            moduledir = path.dirname(modulepath)
            for root, dirs, files in os.walk(modulepath):
                for filename in files:
                    if not filename.endswith('.py'):
                        continue
                    filename = path.join(root[len(moduledir):], filename)
                    filename = filename.lstrip(os.sep)
                    if path.basename(filename) == '__init__.py':
                        filename = path.dirname(filename)
                    yield re.sub('\.py$', '', filename.replace(os.sep, '.'))

    def test_imports(self):
        for modulename in self.find_submodules((cts_core, cts_framework)):
            exec("import " + modulename)

if __name__ == "__main__":
    unittest.main()
