#ifndef PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD
#define PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD

#include <functional>
#include <iostream>

#include <gtest/gtest.h>


extern std::string current_function_name;
extern std::string current_test_name;
extern std::string previous_test_name;
extern std::string expected_value;

inline void set_current_function_name(std::string name) {
    current_function_name = name;
}

inline void set_current_test_name(std::string name) {
    previous_test_name = current_test_name;
    current_test_name = name;
}


struct TestsFinishedException: public std::out_of_range
{
    TestsFinishedException(std::string function_name):
        std::out_of_range(function_name + " called with no remaining tests.")
    {};
};


//
// TestBuilder
//
// TestBuilder runs the next function in a list of functions each time it's
// called.
//
// e.g., for three functions f1, f2, f3, and the program
//     TestBuilder<decltype(f1)> tb;
//     tb.next_test("Calling f1", f1);
//     tb.next_test("Calling f2", f2);
//     tb.next_test("Calling f3", f3);
// we will have the behaviour
//     tb(a, b, c);  //-> calls f1(a, b, c)
//     tb(a, b, c);  //-> calls f2(a, b, c)
//     tb(a, b, c);  //-> calls f3(a, b, c)
//
// If a call raises an exception, the test string will be printed (appended with
// " - Failed" and the exception rethrown.
//

// Template for callable types
template<typename T>
struct TestBuilder: public TestBuilder<decltype(&T::operator())> {
    using TestBuilder<decltype(&T::operator())>::TestBuilder;
};

// Template for class-based callables
template<typename ClassName, typename ReturnType_, typename...Args_>
struct TestBuilder<ReturnType_(ClassName::*)(Args_...) const>:
    public TestBuilder<ReturnType_(Args_...)>
{
    using TestBuilder<ReturnType_(Args_...)>::TestBuilder;
};


// Template for function pointers
template<typename ReturnType_, typename...Args_>
struct TestBuilder<ReturnType_(Args_...)>
{
    typedef ReturnType_ ReturnType;
    typedef std::tuple<Args_...> Args;

    // A list of tests to run each time callback is called.
    typedef std::function<ReturnType_(Args_...)> TestFunction;
    typedef std::pair<std::string, TestFunction> Test;
    typedef std::vector<Test> Tests;
    Tests tests;
    typename Tests::size_type current_test = 0;

    typedef std::function<void()> AllTestsRunCallback;
    AllTestsRunCallback all_tests_run_callback;

    std::string function_name;

    TestBuilder(std::string function_name_): function_name(function_name_) {
        set_current_function_name(function_name_);
    }

    bool all_tests_run()
    {
        return current_test == tests.size();
    }

    template<typename T>
    void next_test(std::string test_string, T test_function)
    {
        if(tests.empty()) {
            set_current_test_name(test_string);
        }

        tests.push_back({test_string, test_function});
    }

    ReturnType operator()(Args_&&...args)
    {
        if(current_test < tests.size()) {
            Test test = tests[current_test];
            current_test++;

            try {
                return do_call<ReturnType_>(test.second, std::forward<Args_>(args)...);
            } catch(...) {
                std::cout << test.first << " - Failed" << std::endl;
                throw;
            }

        } else {
            throw TestsFinishedException(function_name);
        }
    }


    // Call the TestFunction, and call the AllTestsRunCallback after
    // running the last test.
    //
    // The tests needs to run before the callback does.
    // we also need to return the test value after the callback has run.
    //
    // Since some tests return void, some type messing needs to be done so we
    // don't end up taking the reference of a void type with ReturnType& r.
    // std::enable_if is used to enable or disable the do_call function based
    // on the ReturnType template.
    //
    // We can't take ReturnType directly from the class template, because that
    // would result in one undefinable function being defined.
    // Instead, by using the template during the call, the lookup, and so
    // definition only happens for the correct function.
    //

    // Non-void version
    template<typename ReturnType>
    inline typename std::enable_if<not std::is_same<ReturnType, void>::value, ReturnType>::type
    do_call(TestFunction test, Args_&&...args) {
        ReturnType r = test(args...);
        expected_value = ::testing::PrintToString(r);
        if(current_test == tests.size() && all_tests_run_callback)
        {
            all_tests_run_callback();
        } else {
            set_current_test_name(tests[current_test].first);
        }
        return r;
    }

    // Void version
    template<typename ReturnType>
    inline typename std::enable_if<std::is_same<ReturnType, void>::value, void>::type
    do_call(TestFunction test, Args_&&...args) {
        test(args...);
        expected_value = "(void)";
        if(current_test == tests.size() && all_tests_run_callback)
        {
            all_tests_run_callback();
        } else {
            set_current_test_name(tests[current_test].first);
        }
    }
};


template<typename Signature>
TestBuilder<std::function<Signature>>& make_test_builder(
    std::string function_name,
    std::function<Signature>& function,
    std::function<void()> all_tests_run_callback
) {
    typedef std::function<Signature> Function;
    typedef TestBuilder<Function> TestBuilderFunction;

    TestBuilderFunction function_tester(function_name);

    function_tester.all_tests_run_callback = all_tests_run_callback;

    function = function_tester;

    return *(function.template target<TestBuilderFunction>());
}


// Accept and run a set of tests of the form
// void my_test(std::function<void()> callback);
// where callback is called when the last test in my_test is called.
//
// Pops off the first test, and passes running the rest of the tests as
// a callback.
//

// Terminating case
template<typename T = void>
void test_runner() {}

// General case
template<typename Test, typename...Tests>
void test_runner(Test test, Tests... tests) {
    test([tests...]{ test_runner(tests...); });
}

#endif
