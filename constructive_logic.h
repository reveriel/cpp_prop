#ifndef CONSTRUCTIVE_LOGIC_H
#define CONSTRUCTIVE_LOGIC_H

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

// --- Core Definitions ---

// Logical values as types
// 定义逻辑值为类型
struct True {};
struct False {};

// Implies (A → B) as a function
// 蕴含 (Implies): A → B 表示 "若A为真，则B为真"
//    使用 std::function 来表示可调用对象
template <typename A, typename B> using Implies = std::function<B(A)>;

// And (A ∧ B) as a struct
//  合取 (And): A ∧ B 的证明是一个包含 A 的证明和 B 的证明的结构体
template <typename A, typename B> struct And {
  A a;
  B b;
};

// Or (A ∨ B) as a variant
// 3. 析取 (Or): A ∨ B 的证明是 A 或 B 的证明之一
template <typename A, typename B> using Or = std::variant<A, B>;

// Not (¬A) as A → False
// 4. 否定 (Not): ¬A 表示 "A为假"
//    用函数类型表示: A → False
template <typename A> using Not = Implies<A, False>;

// --- Core Constructive Proofs ---

// Modus Ponens: (A, A → B) → B
// 5. 验证假言推理 (Modus Ponens): 若A为真且A→B为真，则B为真, 证明: A ∧ (A → B)
// → B
template <typename A, typename B> B modus_ponens(A a, Implies<A, B> f) {
  return f(a); // 调用函数f，将A类型的a转换为B类型的返回值
}

// And Introduction: A → (B → (A ∧ B))
template <typename A, typename B>
Implies<A, Implies<B, And<A, B>>> and_intro() {
  return [](A a) { return [a](B b) { return And<A, B>{a, b}; }; };
}

// And Elimination (Left): (A ∧ B) → A
// 合取消去 (左)
template <typename A, typename B> Implies<And<A, B>, A> and_elim_left() {
  return [](And<A, B> and_ab) { return and_ab.a; };
}

// And Elimination (Right): (A ∧ B) → B
// 合取消去 (右)
template <typename A, typename B> Implies<And<A, B>, B> and_elim_right() {
  return [](And<A, B> and_ab) { return and_ab.b; };
}

// Or Introduction (Left): A → A ∨ B
// 析取引入 (左)
template <typename A, typename B> Implies<A, Or<A, B>> or_intro_left() {
  return [](A a) { return Or<A, B>{std::in_place_index<0>, a}; };
}

// Or Introduction (Right): B → A ∨ B
// 析取引入 (右)
template <typename A, typename B> Implies<B, Or<A, B>> or_intro_right() {
  return [](B b) { return Or<A, B>{std::in_place_index<1>, b}; };
}

// Or Elimination: (A ∨ B, A → C, B → C) → C
// 析取消去
template <typename A, typename B, typename C>
Implies<Or<A, B>, Implies<Implies<A, C>, Implies<Implies<B, C>, C>>> or_elim() {
  return [](Or<A, B> or_ab) {
    return [or_ab](Implies<A, C> ac) {
      return [or_ab, ac](Implies<B, C> bc) {
        return std::visit(
            [&](auto &&arg) -> C {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, A>) {
                return ac(arg);
              } else {
                return bc(arg);
              }
            },
            or_ab);
      };
    };
  };
}

// Double Negation Introduction: A → ¬¬A
// 验证双重否定引入 (Double Negation Introduction): A → ¬¬A
template <typename A> Implies<A, Not<Not<A>>> double_negation_intro() {
  return [](A a) {         // 假设 A 为真 (premise a)
    return [a](Not<A> f) { // 假设 ¬A (A→False) 为真 (premise f)
      return f(a);         // 则 f(a) 推导出 False，从而证明了 ¬(¬A)
    };
  };
}

// Principle of Explosion (Ex Falso Quodlibet): False → A
template <typename A> Implies<False, A> principle_of_explosion() {
  return [](False f) -> A { throw std::logic_error("Explosion!"); };
}

// 6. 验证三段论: (A→B) ∧ (B→C) → (A→C)
template <typename A, typename B, typename C>
Implies<A, C> syllogism(Implies<A, B> ab, Implies<B, C> bc) {
  return [ab, bc](A a) {
    return bc(ab(a)); // 组合两个函数调用：A→B→C
  };
}

// 6.1 证明三段论恒为真: ((A→B) ∧ (B→C)) → (A→C)
// 在构造性逻辑中，一个定理恒为真，意味着我们可以构造一个函数，
// 该函数可以为任意类型 A, B, C 生成该定理的证明。
// 这个函数本身就是该定理普遍有效性的证明。
template <typename A, typename B, typename C>
Implies<And<Implies<A, B>, Implies<B, C>>, Implies<A, C>> prove_syllogism() {
  return [](And<Implies<A, B>, Implies<B, C>> premises) {
    // 从前提中解构出 A→B 和 B→C
    Implies<A, B> ab = premises.a;
    Implies<B, C> bc = premises.b;
    // 基于前提，构造并返回结论 A→C
    return syllogism(ab, bc);
  };
}

// 6.2
// 柯里化版本: (A→B) → ((B→C) → (A→C))
template <typename A, typename B, typename C>
Implies<Implies<A, B>, Implies<Implies<B, C>, Implies<A, C>>>
prove_syllogism_curried() {
  return [](Implies<A, B> ab) { // 接收第一个前提 P
    return [ab](Implies<B, C> bc) { // 返回一个新函数，接收第二个前提 Q
      return syllogism(ab, bc); // 返回最终结论 R
    };
  };
}

// 8. 验证换质位定律 (Contraposition): (A → B) → (¬B → ¬A)
template <typename A, typename B>
Implies<Implies<A, B>, Implies<Not<B>, Not<A>>> contraposition() {
  return [](Implies<A, B> ab) { // 假设 A → B
    return [ab](Not<B> not_b) { // 假设 ¬B
      return [ab, not_b](A a) { // 假设 A
        B b = ab(a);            // 由 A 和 A→B，得到 B
        return not_b(b); // 由 B 和 ¬B (B→False)，得到 False。从而证明了 ¬A
      };
    };
  };
}

// 9. 验证交换律 (Permutation): (A → (B → C)) → (B → (A → C))
template <typename A, typename B, typename C>
Implies<B, Implies<A, C>> permute(Implies<A, Implies<B, C>> f) {
  return [f](B b) { return [f, b](A a) { return f(a)(b); }; };
}

// --- 定理: AND --- //

// --- 定理: 德摩根定律 (De Morgan's Laws) --- //

// 14. ¬(A ∨ B) → (¬A ∧ ¬B)
template <typename A, typename B>
Implies<Not<Or<A, B>>, And<Not<A>, Not<B>>> de_morgan_1() {
  return [](Not<Or<A, B>> not_or_ab) {
    Not<A> not_a = [not_or_ab](A a) {
      return not_or_ab(or_intro_left<A, B>()(a));
    };
    Not<B> not_b = [not_or_ab](B b) {
      Implies<B, Or<A, B>> or_intro_right = [](B b_in) {
        return Or<A, B>{std::in_place_index<1>, b_in};
      };
      return not_or_ab(or_intro_right(b));
    };
    return And<Not<A>, Not<B>>{not_a, not_b};
  };
}

// 15. 导出规则 (Exportation): ((A ∧ B) → C) → (A → (B → C))
// 对应函数的 curry 化
template <typename A, typename B, typename C>
Implies<A, Implies<B, C>> exportation(Implies<And<A, B>, C> f) {
  return [f](A a) { return [f, a](B b) { return f(And<A, B>{a, b}); }; };
}

// 16. 导入规则 (Importation): (A → (B → C)) → ((A ∧ B) → C)
// 对应函数的反 curry 化
template <typename A, typename B, typename C>
Implies<And<A, B>, C> importation(Implies<A, Implies<B, C>> f) {
  return [f](And<A, B> premises) { return f(premises.a)(premises.b); };
}

// 15. (¬A ∧ ¬B) → ¬(A ∨ B)
template <typename A, typename B>
Implies<And<Not<A>, Not<B>>, Not<Or<A, B>>> de_morgan_2() {
  return [](And<Not<A>, Not<B>> not_a_and_not_b) {
    // We have ¬A and ¬B from the premise
    Not<A> not_a = and_elim_left<Not<A>, Not<B>>()(not_a_and_not_b);
    Not<B> not_b = and_elim_right<Not<A>, Not<B>>()(not_a_and_not_b);

    // We want to prove ¬(A ∨ B), which is (A ∨ B) → False.
    // So, we assume (A ∨ B) and try to derive False.
    return [not_a, not_b](Or<A, B> or_ab) {
      // We can use or_elim. It needs a proof of A→C and B→C.
      // Here, C is False. So we need A→False (¬A) and B→False (¬B), which we
      // have.
      Implies<Or<A, B>,
              Implies<Implies<A, False>, Implies<Implies<B, False>, False>>>
          elim = or_elim<A, B, False>();
      return elim(or_ab)(not_a)(not_b);
    };
  };
}

// 16. (A → B) → ((A → ¬B) → ¬A) (Reductio ad Absurdum)
template <typename A, typename B>
Implies<Implies<A, B>, Implies<Implies<A, Not<B>>, Not<A>>>
reductio_ad_absurdum() {
  return [](Implies<A, B> a_implies_b) {
    return [a_implies_b](Implies<A, Not<B>> a_implies_not_b) {
      // We want to prove ¬A, which is A → False.
      // So, we assume A and try to derive False.
      return [a_implies_b, a_implies_not_b](A a) {
        // From A and A→B, we get B.
        B b = modus_ponens(a, a_implies_b);
        // From A and A→¬B, we get ¬B (which is B → False).
        Not<B> not_b = modus_ponens(a, a_implies_not_b);
        // From B and ¬B (B → False), we get False.
        return modus_ponens(b, not_b);
      };
    };
  };
}

#endif // CONSTRUCTIVE_LOGIC_H
