import unittest2 as unittest
import unit_tests

if __name__ == "__main__":
    all_tests = unittest.TestLoader().loadTestsFromModule(unit_tests)
    unittest.TextTestRunner(verbosity=2).run(all_tests)
