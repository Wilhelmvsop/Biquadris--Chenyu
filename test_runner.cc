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
void assert_true(bool condition, const std::string& message);

template <typename T>
void assert_equal(const T& expected, const T& actual,
                  const std::string& message = "") {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: expected != actual. " +
                                 message);
    }
}

int run_all_tests();
}  // namespace Tester

// Macros AFTER the module interface
#define TEST_CASE(name)                                                       \
    static void test_##name();                                                \
    namespace {                                                               \
    struct TestRegistrar_##name {                                             \
        TestRegistrar_##name() { Tester::register_test(#name, test_##name); } \
    } registrar_##name;                                                       \
    }                                                                         \
    static void test_##name()

#define REQUIRE(condition) Tester::assert_true(condition, #condition)

#define REQUIRE_EQUAL(expected, actual) \
    Tester::assert_equal(expected, actual, #expected " == " #actual)
