#include <gtest/gtest.h>
#include "../template.h"

using namespace cpp_prop;

// Test Fixture for Propositional Logic Types
class LogicTest : public ::testing::Test {};

TEST_F(LogicTest, And) {
    ASSERT_TRUE((std::is_same_v<And<TrueType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<And<TrueType, FalseType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<And<FalseType, TrueType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<And<FalseType, FalseType>::type, FalseType>));
}

TEST_F(LogicTest, Or) {
    ASSERT_TRUE((std::is_same_v<Or<TrueType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Or<TrueType, FalseType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Or<FalseType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Or<FalseType, FalseType>::type, FalseType>));
}

TEST_F(LogicTest, Not) {
    ASSERT_TRUE((std::is_same_v<Not<TrueType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<Not<FalseType>::type, TrueType>));
}

TEST_F(LogicTest, Implies) {
    ASSERT_TRUE((std::is_same_v<Implies<TrueType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Implies<TrueType, FalseType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<Implies<FalseType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Implies<FalseType, FalseType>::type, TrueType>));
}

TEST_F(LogicTest, Equiv) {
    ASSERT_TRUE((std::is_same_v<Equiv<TrueType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Equiv<TrueType, FalseType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<Equiv<FalseType, TrueType>::type, FalseType>));
    ASSERT_TRUE((std::is_same_v<Equiv<FalseType, FalseType>::type, TrueType>));
}

TEST_F(LogicTest, Syllogism) {
    ASSERT_TRUE((std::is_same_v<Syllogism<TrueType, TrueType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Syllogism<TrueType, TrueType, FalseType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Syllogism<TrueType, FalseType, TrueType>::type, TrueType>));
    ASSERT_TRUE((std::is_same_v<Syllogism<FalseType, TrueType, TrueType>::type, TrueType>));
}
