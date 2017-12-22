import unittest2 as unittest
import unit_tests
import sys
from colour_runner.runner import ColourTextTestRunner

def main():
    all_tests = unittest.TestLoader().loadTestsFromModule(unit_tests)
    runner = ColourTextTestRunner(buffer=True, verbosity=2)
    success = runner.run(all_tests).wasSuccessful()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
