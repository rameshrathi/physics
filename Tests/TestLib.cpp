
#include "TestLib.h"

using namespace TestLib;
using namespace TestLib::Assert;

// Example fixture
class MathFixture : public TestFixture {
public:
    int a, b;
    
    void SetUp() override {
        a = 5;
        b = 3;
    }
    
    void TearDown() override {
        // Cleanup if needed
    }
    
    void TestAddition() {
        AreEqual(8, a + b, "Addition test");
    }
    
    void TestSubtraction() {
        AreEqual(2, a - b, "Subtraction test");
    }
};

// Test suite definition
TEST_SUITE(MathTests)
    void RegisterTests(TestSuite& suite) override {
        TEST_CASE_WITH_FIXTURE(TestAddition, MathFixture);
        TEST_CASE_WITH_FIXTURE(TestSubtraction, MathFixture);
        TEST_CASE(TestSimple);
    }
    
    void TestSimple() {
        AreEqual(4, 2 + 2);
        IsTrue(true);
        IsFalse(false);
    }
END_SUITE()

// Simple standalone test
SIMPLE_TEST(BasicTests, StringTest, {
    std::string str = "Hello";
    AreEqual("Hello", str);
    AreEqual(5u, str.length());
})

int main() {
    TestRunner runner(true);
    
    // Add test suites
    runner.AddSuite(MathTests_Suite::Create());
    
    // Add standalone test
    auto basic_suite = std::make_unique<TestSuite>("BasicTests");
    basic_suite->AddTest(std::make_unique<TestCase>("StringTest", BasicTests_StringTest));
    runner.AddSuite(std::move(basic_suite));
    
    // Run all tests
    runner.RunAll();
    
    return 0;
}
