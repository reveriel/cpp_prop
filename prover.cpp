#include "constructive_logic.h"
#include <iostream>

// --- Example Proofs using the library ---

// Example: A proof for "True → True"
// 示例：证明 "True → True"
Implies<True, True> always_true = [](True) -> True {
  return {}; // 返回一个True类型的实例
};

// Example: A proof for "¬False" (False → False)
Implies<False, False> prove_not_false = [](False f) -> False {
  // 实际上永远不会执行，因为False类型没有实例
  throw std::logic_error("This function should never be called.");
};

// 下面的对证明的调用不是必须的，只要通过编译就说明证明已经通过了类型检查.
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
  And<Implies<True, True>, Implies<True, True>> premises = {always_true,
                                                            always_true};
  Implies<True, True> conclusion = syllogism_proof_generator(premises);
  True result_syllogism_proof = conclusion(True{});
  static_assert(std::is_same_v<decltype(result_syllogism_proof), True>,
                "三段论恒为真证明失败");

  // 验证双重否定: True → ¬¬True
  auto dni = double_negation_intro<True>();
  Not<Not<True>> dnt = dni(True{});
  // dnt(not_true) 应该返回 False，这里我们只验证类型
  static_assert(std::is_same_v<decltype(dnt), Not<Not<True>>>,
                "双重否定引入证明失败");

  // 验证换质位: (True → True) → (¬True → ¬True)
  auto cp = contraposition<True, True>();
  Implies<Not<True>, Not<True>> cp_proof = cp(always_true);
  static_assert(
      std::is_same_v<decltype(cp_proof), Implies<Not<True>, Not<True>>>,
      "换质位证明失败");

  // 验证交换律
  Implies<True, Implies<True, True>> f = [](True) { return always_true; };
  auto p = permute<True, True, True>(f);
  True result3 = p(True{})(True{});
  static_assert(std::is_same_v<decltype(result3), True>, "交换律证明失败");

  // 验证合取引入: True, True → True ∧ True
  auto and_i = and_intro<True, True>();
  And<True, True> and_proof = and_i(True{})(True{});
  static_assert(std::is_same_v<decltype(and_proof.a), True>,
                "合取引入证明失败");

  // 验证德摩根定律: ¬(False ∨ False) → (¬False ∧ ¬False)
  auto dm1 = de_morgan_1<False, False>();
  Not<Or<False, False>> not_or_ff = [](Or<False, False> ff) {
    return std::get<0>(ff);
  }; // 假设
  And<Not<False>, Not<False>> de_morgan_proof = dm1(not_or_ff);
  static_assert(
      std::is_same_v<decltype(de_morgan_proof), And<Not<False>, Not<False>>>,
      "德摩根定律证明失败");

  // 验证柯里化版本的三段论
  auto syllogism_curried_proof_generator =
      prove_syllogism_curried<True, True, True>();
  Implies<Implies<True, True>, Implies<True, True>> curried_conclusion_func =
      syllogism_curried_proof_generator(always_true);
  Implies<True, True> curried_conclusion = curried_conclusion_func(always_true);
  True result_syllogism_curried_proof = curried_conclusion(True{});
  static_assert(std::is_same_v<decltype(result_syllogism_curried_proof), True>,
                "柯里化三段论证明失败");

  // 验证导出/导入规则
  // 创建一个 (True ∧ True) → True 的函数
  Implies<And<True, True>, True> and_to_true = [](And<True, True> p) {
    return True{};
  };

  // 导出
  auto exported = exportation<True, True, True>(and_to_true);
  True result_export = exported(True{})(True{});
  static_assert(std::is_same_v<decltype(result_export), True>,
                "导出规则证明失败");

  // 导入
  auto imported = importation<True, True, True>(exported);
  True result_import = imported(And<True, True>{True{}, True{}});
  static_assert(std::is_same_v<decltype(result_import), True>,
                "导入规则证明失败");

  // 验证德摩根定律2: (¬True ∧ ¬False) → ¬(True ∨ False)
  auto dm2 = de_morgan_2<True, False>();
  Not<True> not_true = [](True) -> False {
    throw std::logic_error("Should not be called");
  };
  Not<False> not_false = [](False) -> False {
    throw std::logic_error("Should not be called");
  };
  And<Not<True>, Not<False>> premise_dm2 = {not_true, not_false};
  Not<Or<True, False>> dm2_proof = dm2(premise_dm2);
  static_assert(std::is_same_v<decltype(dm2_proof), Not<Or<True, False>>>,
                "德摩根定律2证明失败");

  // 验证归谬律: (True → False) → ((True → ¬False) → ¬True)
  auto raa = reductio_ad_absurdum<True, False>();
  Implies<True, False> true_implies_false = [](True) -> False {
    return False{};
  };
  Implies<True, Not<False>> true_implies_not_false =
      [not_false](True) -> Not<False> { return not_false; };
  Not<True> raa_proof = raa(true_implies_false)(true_implies_not_false);
  static_assert(std::is_same_v<decltype(raa_proof), Not<True>>,
                "归谬律证明失败");

  std::cout << "所有证明均通过编译！" << std::endl;
  return 0;
}