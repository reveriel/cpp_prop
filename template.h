#include <type_traits>

namespace cpp_prop {

// 基础逻辑类型
struct TrueType {};
struct FalseType {};

// 合取：A ∧ B 为真，当且仅当A和B都为真
template <typename A, typename B> struct And {
  // 默认情况：A或B为假，则整体为假
  using type = FalseType;
};
// 特化：A和B都为真时，整体为真
template <> struct And<TrueType, TrueType> {
  using type = TrueType;
};

// 验证：True ∧ True → True
static_assert(std::is_same_v<And<TrueType, TrueType>::type, TrueType>,
              "And: True ∧ True 应为真");
// 验证：True ∧ False → False
static_assert(std::is_same_v<And<TrueType, FalseType>::type, FalseType>,
              "And: True ∧ False 应为假");

// 析取：A ∨ B 为真，当且仅当A或B至少一个为真
template <typename A, typename B> struct Or {
  // 默认情况：A和B都为假，则整体为假
  using type = FalseType;
};
// 特化：A为真时，整体为真
template <typename B> struct Or<TrueType, B> {
  using type = TrueType;
};
// 特化：B为真时，整体为真（即使A为假）

template <typename A> struct Or<A, TrueType> {
  using type = TrueType;
};
// 特化：解决当A和B都为TrueType时的歧义

template <> struct Or<TrueType, TrueType> {
  using type = TrueType;
};

// 验证：True ∨ False → True
static_assert(std::is_same_v<Or<TrueType, FalseType>::type, TrueType>,
              "Or: True ∨ False 应为真");
// 验证：False ∨ False → False
static_assert(std::is_same_v<Or<FalseType, FalseType>::type, FalseType>,
              "Or: False ∨ False 应为假");

// 否定：¬A 为真，当且仅当A为假
template <typename A> struct Not {
  // 默认情况：A为真，则否定为假
  using type = FalseType;
};
// 特化：A为假时，否定为真
template <> struct Not<FalseType> {
  using type = TrueType;
};

// 验证：¬True → False
static_assert(std::is_same_v<Not<TrueType>::type, FalseType>,
              "Not: ¬True 应为假");
// 验证：¬False → True
static_assert(std::is_same_v<Not<FalseType>::type, TrueType>,
              "Not: ¬False 应为真");

// 蕴含：A → B 为真，当且仅当A为假或B为真（之前的IfThen）
template <typename A, typename B> struct Implies {
  using type = TrueType; // 默认：A为假时，蕴含为真
};
template <typename B> struct Implies<TrueType, B> {
  using type = B; // A为真时，蕴含结果等于B
};

// 等价：A ↔ B 等价于 (A→B) ∧ (B→A)
template <typename A, typename B> struct Equiv {
  using type = typename And<typename Implies<A, B>::type,
                            typename Implies<B, A>::type>::type;
};

// 验证：True ↔ True → True
static_assert(std::is_same_v<Equiv<TrueType, TrueType>::type, TrueType>,
              "Equiv: True ↔ True 应为真");
// 验证：True ↔ False → False
static_assert(std::is_same_v<Equiv<TrueType, FalseType>::type, FalseType>,
              "Equiv: True ↔ False 应为假");

// 三段论：(A→B) ∧ (B→C) → (A→C)
template <typename A, typename B, typename C> struct Syllogism {
  // 先验证A→B和B→C是否为真
  using AB = typename Implies<A, B>::type;
  using BC = typename Implies<B, C>::type;
  // 若A→B和B→C都为真，则结论A→C为真
  using type =
      typename Implies<typename And<AB, BC>::type,  // 前提：(A→B) ∧ (B→C)
                       typename Implies<A, C>::type // 结论：A→C
                       >::type;
};

// --- 验证三段论是重言式 ---
// 通过静态断言在编译期检查所有可能的真值组合 (2^3 = 8)
// 只要有一个组合不为 TrueType，编译就会失败
static_assert(
    std::is_same_v<Syllogism<TrueType, TrueType, TrueType>::type, TrueType>,
    "Syllogism failed for TTT");
static_assert(
    std::is_same_v<Syllogism<TrueType, TrueType, FalseType>::type, TrueType>,
    "Syllogism failed for TTF");
static_assert(
    std::is_same_v<Syllogism<TrueType, FalseType, TrueType>::type, TrueType>,
    "Syllogism failed for TFT");
static_assert(
    std::is_same_v<Syllogism<TrueType, FalseType, FalseType>::type, TrueType>,
    "Syllogism failed for TFF");
static_assert(
    std::is_same_v<Syllogism<FalseType, TrueType, TrueType>::type, TrueType>,
    "Syllogism failed for FTT");
static_assert(
    std::is_same_v<Syllogism<FalseType, TrueType, FalseType>::type, TrueType>,
    "Syllogism failed for FTF");
static_assert(
    std::is_same_v<Syllogism<FalseType, FalseType, TrueType>::type, TrueType>,
    "Syllogism failed for FFT");
static_assert(
    std::is_same_v<Syllogism<FalseType, FalseType, FalseType>::type, TrueType>,
    "Syllogism failed for FFF");

} // namespace cpp_prop
