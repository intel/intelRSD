import unittest2 as unittest
import unit_tests
from colour_runner.runner import ColourTextTestRunner

if __name__ == "__main__":
    all_tests = unittest.TestLoader().loadTestsFromModule(unit_tests)
    ColourTextTestRunner(buffer=True, verbosity=2).run(all_tests)