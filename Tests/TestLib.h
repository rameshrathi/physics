#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <chrono>
#include <memory>
#include <map>
#include <exception>

namespace TestLib {

// Forward declarations
class TestCase;
class TestSuite;
class TestRunner;

// Exception for test failures
class AssertionFailure : public std::exception {
private:
    std::string message_;
public:
    explicit AssertionFailure(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string error_message;
    std::chrono::milliseconds duration;
    
    TestResult(const std::string& n, bool p, const std::string& err = "", 
               std::chrono::milliseconds dur = std::chrono::milliseconds(0))
        : name(n), passed(p), error_message(err), duration(dur) {}
};

// Base test fixture class
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
};

// Test case class
class TestCase {
private:
    std::string name_;
    std::function<void()> test_func_;
    std::shared_ptr<TestFixture> fixture_;

public:
    TestCase(const std::string& name, std::function<void()> func, 
             std::shared_ptr<TestFixture> fixture = nullptr)
        : name_(name), test_func_(func), fixture_(fixture) {}

    TestResult Run() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            if (fixture_) fixture_->SetUp();
            test_func_();
            if (fixture_) fixture_->TearDown();
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            return TestResult(name_, true, "", duration);
        }
        catch (const AssertionFailure& e) {
            if (fixture_) fixture_->TearDown();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            return TestResult(name_, false, e.what(), duration);
        }
        catch (const std::exception& e) {
            if (fixture_) fixture_->TearDown();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            return TestResult(name_, false, std::string("Unexpected exception: ") + e.what(), duration);
        }
        catch (...) {
            if (fixture_) fixture_->TearDown();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            return TestResult(name_, false, "Unknown exception occurred", duration);
        }
    }

    const std::string& GetName() const { return name_; }
};

// Test suite class
class TestSuite {
private:
    std::string name_;
    std::vector<std::unique_ptr<TestCase>> tests_;

public:
    explicit TestSuite(const std::string& name) : name_(name) {}

    void AddTest(std::unique_ptr<TestCase> test) {
        tests_.push_back(std::move(test));
    }

    std::vector<TestResult> RunAll() {
        std::vector<TestResult> results;
        for (auto& test : tests_) {
            results.push_back(test->Run());
        }
        return results;
    }

    const std::string& GetName() const { return name_; }
    size_t GetTestCount() const { return tests_.size(); }
};

// Test runner class
class TestRunner {
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
    bool verbose_;

public:
    TestRunner(bool verbose = true) : verbose_(verbose) {}

    void AddSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }

    void RunAll() {
        int total_tests = 0;
        int passed_tests = 0;
        std::chrono::milliseconds total_duration(0);

        std::cout << "Running tests...\n" << std::string(50, '=') << "\n\n";

        for (auto& suite : suites_) {
            std::cout << "Test Suite: " << suite->GetName() << "\n";
            
            auto results = suite->RunAll();
            total_tests += results.size();

            for (const auto& result : results) {
                total_duration += result.duration;
                
                if (result.passed) {
                    passed_tests++;
                    if (verbose_) {
                        std::cout << "  ✓ " << result.name << " (" << result.duration.count() << "ms)\n";
                    }
                } else {
                    std::cout << "  ✗ " << result.name << " (" << result.duration.count() << "ms)\n";
                    std::cout << "    Error: " << result.error_message << "\n";
                }
            }
            std::cout << "\n";
        }

        // Summary
        std::cout << std::string(50, '=') << "\n";
        std::cout << "Test Summary:\n";
        std::cout << "  Total tests: " << total_tests << "\n";
        std::cout << "  Passed: " << passed_tests << "\n";
        std::cout << "  Failed: " << (total_tests - passed_tests) << "\n";
        std::cout << "  Total time: " << total_duration.count() << "ms\n";
        std::cout << "  Success rate: " << (total_tests > 0 ? (passed_tests * 100.0 / total_tests) : 0) << "%\n";
    }
};

// Assertion macros and functions
namespace Assert {
    
    template<typename T, typename U>
    void AreEqual(const T& expected, const U& actual, const std::string& message = "") {
        if (!(expected == actual)) {
            std::ostringstream oss;
            oss << "Expected: " << expected << ", Actual: " << actual;
            if (!message.empty()) {
                oss << " (" << message << ")";
            }
            throw AssertionFailure(oss.str());
        }
    }

    template<typename T, typename U>
    void AreNotEqual(const T& expected, const U& actual, const std::string& message = "") {
        if (expected == actual) {
            std::ostringstream oss;
            oss << "Expected values to be different, but both are: " << expected;
            if (!message.empty()) {
                oss << " (" << message << ")";
            }
            throw AssertionFailure(oss.str());
        }
    }

    void IsTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            std::string error = "Expected: true, Actual: false";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
    }

    void IsFalse(bool condition, const std::string& message = "") {
        if (condition) {
            std::string error = "Expected: false, Actual: true";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
    }

    template<typename T>
    void IsNull(T* pointer, const std::string& message = "") {
        if (pointer != nullptr) {
            std::string error = "Expected pointer to be null";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
    }

    template<typename T>
    void IsNotNull(T* pointer, const std::string& message = "") {
        if (pointer == nullptr) {
            std::string error = "Expected pointer to not be null";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
    }

    template<typename Func>
    void Throws(Func&& func, const std::string& message = "") {
        try {
            func();
            std::string error = "Expected exception to be thrown";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
        catch (const AssertionFailure&) {
            throw; // Re-throw assertion failures
        }
        catch (...) {
            // Expected exception was thrown
        }
    }

    template<typename ExceptionType, typename Func>
    void ThrowsType(Func&& func, const std::string& message = "") {
        try {
            func();
            std::string error = "Expected exception of specific type to be thrown";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
        catch (const ExceptionType&) {
            // Expected exception type was thrown
            return;
        }
        catch (const AssertionFailure&) {
            throw; // Re-throw assertion failures
        }
        catch (...) {
            std::string error = "Wrong exception type was thrown";
            if (!message.empty()) {
                error += " (" + message + ")";
            }
            throw AssertionFailure(error);
        }
    }

    void Fail(const std::string& message = "Test failed") {
        throw AssertionFailure(message);
    }
}

// Helper macros for easier test definition
#define TEST_SUITE(name) \
    class name##_Suite { \
    public: \
        static std::unique_ptr<TestSuite> Create() { \
            auto suite = std::make_unique<TestSuite>(#name); \
            name##_Suite instance; \
            instance.RegisterTests(*suite); \
            return suite; \
        } \
    private: \
        void RegisterTests(TestSuite& suite);

#define TEST_CASE(name) \
    suite.AddTest(std::make_unique<TestCase>(#name, [this]() { this->name(); }));

#define TEST_CASE_WITH_FIXTURE(name, fixture_type) \
    suite.AddTest(std::make_unique<TestCase>(#name, [fixture = std::make_shared<fixture_type>()]() { \
        fixture->name(); \
    }, std::static_pointer_cast<TestFixture>(std::make_shared<fixture_type>())));

#define END_SUITE() \
    };

// Standalone test function helper
#define SIMPLE_TEST(suite_name, test_name, test_body) \
    void suite_name##_##test_name() test_body

} // namespace TestLib
