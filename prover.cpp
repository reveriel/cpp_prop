#include <iostream>
#include <functional> // For std::function
#include <variant>    // For Or
#include <utility>    // For std::decay_t
#include <stdexcept>  // For std::logic_error
#include <type_traits> // For std::is_same_v

// 定义逻辑值为类型
struct True {};
struct False {};

// 1. 蕴含 (Implies): A → B 表示 "若A为真，则B为真"
//    使用 std::function 来表示可调用对象
template <typename A, typename B>
using Implies = std::function<B(A)>;

// 2. 合取 (And): A ∧ B 的证明是一个包含 A 的证明和 B 的证明的结构体
template <typename A, typename B>
struct And {
  A a;
  B b;
};

// 3. 析取 (Or): A ∨ B 的证明是 A 或 B 的证明之一
template <typename A, typename B> using Or = std::variant<A, B>;

// 4. 否定 (Not): ¬A 表示 "A为假"
//    用函数类型表示: A → False
template <typename A> using Not = Implies<A, False>;

// 5. 验证假言推理 (Modus Ponens): 若A为真且A→B为真，则B为真, 证明: A ∧ (A → B)
// → B
template <typename A, typename B> B modus_ponens(A a, Implies<A, B> f) {
  return f(a); // 调用函数f，将A类型的a转换为B类型的返回值
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
auto prove_syllogism_curried() {
  return [](Implies<A, B> ab) { // 接收第一个前提 P
    return [ab](Implies<B, C> bc) { // 返回一个新函数，接收第二个前提 Q
      return syllogism(ab, bc); // 返回最终结论 R
    };
  };
}

// 7. 验证双重否定引入 (Double Negation Introduction): A → ¬¬A
template <typename A>
Implies<A, Not<Not<A>>> double_negation_intro() {
  return [](A a) {         // 假设 A 为真 (premise a)
    return [a](Not<A> f) { // 假设 ¬A (A→False) 为真 (premise f)
      return f(a);         // 则 f(a) 推导出 False，从而证明了 ¬(¬A)
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
        return not_b(b);        // 由 B 和 ¬B (B→False)，得到 False。从而证明了 ¬A
      };
    };
  };
}

// 9. 验证交换律 (Permutation): (A → (B → C)) → (B → (A → C))
template <typename A, typename B, typename C>
Implies<B, Implies<A, C>> permute(Implies<A, Implies<B, C>> f) {
    return [f](B b) {
        return [f, b](A a) {
            return f(a)(b);
        };
    };
}


// --- 定理: AND --- //

// 10. 合取引入 (And Introduction): A, B → A ∧ B
template <typename A, typename B>
Implies<A, Implies<B, And<A, B>>> and_intro() {
  return [](A a) {
    return [a](B b) {
      return And<A, B>{a, b};
    };
  };
}

// 11. 合取消去 (左) (And Elimination Left): A ∧ B → A
template <typename A, typename B>
Implies<And<A, B>, A> and_elim_left() {
  return [](And<A, B> and_ab) {
    return and_ab.a;
  };
}

// --- 定理: OR --- //

// 12. 析取引入 (左) (Or Introduction Left): A → A ∨ B
template <typename A, typename B>
Implies<A, Or<A, B>> or_intro_left() {
  return [](A a) {
    return Or<A, B>{std::in_place_index<0>, a};
  };
}

// 13. 析取消去 (Or Elimination): (A∨B, A→C, B→C) → C
template <typename A, typename B, typename C>
Implies<Or<A, B>, Implies<Implies<A, C>, Implies<Implies<B, C>, C>>> or_elim() {
  return [](Or<A, B> or_ab) {
    return [or_ab](Implies<A, C> ac) {
      return [or_ab, ac](Implies<B, C> bc) {
        return std::visit([&](auto&& arg) -> C {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, A>) {
            return ac(arg);
          } else {
            return bc(arg);
          }
        }, or_ab);
      };
    };
  };
}

// --- 定理: 德摩根定律 (De Morgan's Laws) --- //

// 14. ¬(A ∨ B) → (¬A ∧ ¬B)
template <typename A, typename B>
Implies<Not<Or<A, B>>, And<Not<A>, Not<B>>> de_morgan_1() {
  return [](Not<Or<A, B>> not_or_ab) {
    Not<A> not_a = [not_or_ab](A a) { return not_or_ab(or_intro_left<A, B>()(a)); };
    Not<B> not_b = [not_or_ab](B b) {
        Implies<B, Or<A, B>> or_intro_right = [](B b_in){ return Or<A, B>{std::in_place_index<1>, b_in}; };
        return not_or_ab(or_intro_right(b));
    };
    return And<Not<A>, Not<B>>{not_a, not_b};
  };
}

// 15. 导出规则 (Exportation): ((A ∧ B) → C) → (A → (B → C))
template <typename A, typename B, typename C>
Implies<A, Implies<B, C>> exportation(Implies<And<A, B>, C> f) {
    return [f](A a) {
        return [f, a](B b) {
            return f(And<A, B>{a, b});
        };
    };
}

// 16. 导入规则 (Importation): (A → (B → C)) → ((A ∧ B) → C)
template <typename A, typename B, typename C>
Implies<And<A, B>, C> importation(Implies<A, Implies<B, C>> f) {
    return [f](And<A, B> premises) {
        return f(premises.a)(premises.b);
    };
}

// 示例：证明 "True → True"
Implies<True, True> always_true = [](True) -> True {
  return {}; // 返回一个True类型的实例
};

// 示例：证明 "¬False" (即 False → False)
Implies<False, False> prove_not_false = [](False f) -> False {
  // 实际上永远不会执行，因为False类型没有实例
  throw std::logic_error("False类型无法实例化");
};

int main() {
  // 验证假言推理：若True为真且True→True为真，则True为真
  True result = modus_ponens(True{}, always_true);
  static_assert(std::is_same_v<decltype(result), True>, "证明失败");

  // 验证三段论：若True→True且True→True，则True→True
  auto proof = syllogism(always_true, always_true);
  True result2 = proof(True{});
  static_assert(std::is_same_v<decltype(result2), True>, "证明失败");

  // 验证三段论恒为真
  auto syllogism_proof_generator = prove_syllogism<True, True, True>();
  And<Implies<True, True>, Implies<True, True>> premises = {always_true, always_true};
  Implies<True, True> conclusion = syllogism_proof_generator(premises);
  True result_syllogism_proof = conclusion(True{});
  static_assert(std::is_same_v<decltype(result_syllogism_proof), True>, "三段论恒为真证明失败");

  // 验证双重否定: True → ¬¬True
  auto dni = double_negation_intro<True>();
  Not<Not<True>> dnt = dni(True{});
  // dnt(not_true) 应该返回 False，这里我们只验证类型
  static_assert(std::is_same_v<decltype(dnt), Not<Not<True>>>, "双重否定引入证明失败");

  // 验证换质位: (True → True) → (¬True → ¬True)
  auto cp = contraposition<True, True>();
  Implies<Not<True>, Not<True>> cp_proof = cp(always_true);
  static_assert(std::is_same_v<decltype(cp_proof), Implies<Not<True>, Not<True>>>, "换质位证明失败");

  // 验证交换律
  Implies<True, Implies<True, True>> f = [](True){ return always_true; };
  auto p = permute<True, True, True>(f);
  True result3 = p(True{})(True{});
  static_assert(std::is_same_v<decltype(result3), True>, "交换律证明失败");

  // 验证合取引入: True, True → True ∧ True
  auto and_i = and_intro<True, True>();
  And<True, True> and_proof = and_i(True{})(True{});
  static_assert(std::is_same_v<decltype(and_proof.a), True>, "合取引入证明失败");

  // 验证德摩根定律: ¬(False ∨ False) → (¬False ∧ ¬False)
  auto dm1 = de_morgan_1<False, False>();
  Not<Or<False, False>> not_or_ff = [](Or<False, False> ff){ return std::get<0>(ff); }; // 假设
  And<Not<False>, Not<False>> de_morgan_proof = dm1(not_or_ff);
  static_assert(std::is_same_v<decltype(de_morgan_proof), And<Not<False>, Not<False>>>, "德摩根定律证明失败");

  // 验证柯里化版本的三段论
  auto syllogism_curried_proof_generator = prove_syllogism_curried<True, True, True>();
  Implies<Implies<True, True>, Implies<True, True>> curried_conclusion_func = syllogism_curried_proof_generator(always_true);
  Implies<True, True> curried_conclusion = curried_conclusion_func(always_true);
  True result_syllogism_curried_proof = curried_conclusion(True{});
  static_assert(std::is_same_v<decltype(result_syllogism_curried_proof), True>, "柯里化三段论证明失败");

  // 验证导出/导入规则
  // 创建一个 (True ∧ True) → True 的函数
  Implies<And<True, True>, True> and_to_true = [](And<True, True> p) { return True{}; };

  // 导出
  auto exported = exportation<True, True, True>(and_to_true);
  True result_export = exported(True{})(True{});
  static_assert(std::is_same_v<decltype(result_export), True>, "导出规则证明失败");

  // 导入
  auto imported = importation<True, True, True>(exported);
  True result_import = imported(And<True, True>{True{}, True{}});
  static_assert(std::is_same_v<decltype(result_import), True>, "导入规则证明失败");

  std::cout << "所有证明均通过编译！" << std::endl;
  return 0;
}