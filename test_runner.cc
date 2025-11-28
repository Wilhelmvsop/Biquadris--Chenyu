export module TestRunner;

import <iostream>;
import <vector>;
import <string>;
import <functional>;
import <stdexcept>;

export namespace Tester {
struct TestCase {
    std::string name;
    std::function<void()> test_func;
};

std::vector<TestCase>& get_tests();
void register_test(const std::string& name, std::function<void()> func);
void assert_true(bool condition, const std::string& message = "");

template <typename T>
void assert_equal(const T& expected, const T& actual,
                  const std::string& message = "") {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: expected != actual. " +
                                 message);
    }
}

int run_all_tests();

// Helper class for automatic test registration
struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> func) {
        register_test(name, func);
    }
};
}  // namespace Tester
