// tests/test_math.cc
// Tests for math module

import math; // Your math module
import <iostream>;

#include "test_runner.h"

TEST_CASE(Addition_works_correctly) {
    Calculator calc;
    REQUIRE_EQUAL(4, calc.add(2, 2));
    REQUIRE_EQUAL(0, calc.add(-1, 1));
    REQUIRE_EQUAL(10, calc.add(7, 3));

    // or
    REQUIRE(4 == calc.add(2, 2));
    REQUIRE(1 == calc.add(-1, 1));
}
