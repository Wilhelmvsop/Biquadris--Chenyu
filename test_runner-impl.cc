module TestRunner;

namespace Tester {
std::vector<TestCase>& get_tests() {
    static std::vector<TestCase> tests;
    return tests;
}

void register_test(const std::string& name, std::function<void()> func) {
    get_tests().push_back({name, func});
}

void assert_true(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

int run_all_tests() {
    int passed = 0;
    int failed = 0;

    std::cout << "Running " << get_tests().size() << " tests...\n\n";

    for (const auto& test : get_tests()) {
        try {
            std::cout << "  " << test.name << " ... ";
            test.test_func();
            std::cout << "PASSED\n";
            passed++;
        } catch (const std::exception& e) {
            std::cout << "FAILED\n";
            std::cout << "    " << e.what() << "\n";
            failed++;
        }
    }

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
}  // namespace Tester
