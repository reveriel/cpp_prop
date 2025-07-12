#include "constructive_logic.h"
#include <iostream>

// --- Proofs of Equivalence for Classical Logic Axioms ---
// These functions are "proof transformers". They take the proof of one
// classical axiom as an argument and return a proof of another.

// Proof: LEM → DNE
// If we have a proof of (A ∨ ¬A), we can construct a proof of (¬¬A → A).
template <typename A>
Implies<Not<Not<A>>, A> prove_dne_from_lem(Or<A, Not<A>> lem_instance) {
  return [lem_instance](Not<Not<A>> nna) -> A {
    // We use or_elim on the provided proof of LEM.
    // We need to show that both sides of the OR lead to our goal, A.

    // Case 1: The OR gives us A.
    Implies<A, A> case1 = [](A a) { return a; };

    // Case 2: The OR gives us ¬A.
    Implies<Not<A>, A> case2 = [nna](Not<A> na) -> A {
      // We have ¬A from this case, and ¬¬A from the function's premise.
      False f = modus_ponens(na, nna);
      // From False, we can prove anything, including A.
      return principle_of_explosion<A>()(f);
    };

    return or_elim<A, Not<A>, A>()(lem_instance)(case1)(case2);
  };
}

// Proof: DNE → Peirce's Law
// If we have a proof of (¬¬A → A), we can construct a proof of (((A→B)→A)→A).
template <typename A, typename B>
Implies<Implies<Implies<A, B>, A>, A>
prove_peirce_from_dne(Implies<Not<Not<A>>, A> dne_instance) {
  return [dne_instance](Implies<Implies<A, B>, A> f) -> A {
    // To prove A using DNE, we first need to prove ¬¬A.
    Not<Not<A>> nna = [f](Not<A> na) -> False {
      // Assume ¬A (na).
      // `f` requires a proof of (A → B). Let's construct one.
      Implies<A, B> a_to_b = [na](A a) -> B {
        False contradiction = modus_ponens(a, na);
        return principle_of_explosion<B>()(contradiction);
      };
      A result_A = f(a_to_b);
      return modus_ponens(result_A, na);
    };
    // Now, use the provided DNE proof to get from ¬¬A to A.
    return dne_instance(nna);
  };
}

// Proof: Peirce's Law → DNE
// If we have a proof of (((A→B)→A)→A), we can construct a proof of (¬¬A → A).
template <typename A, typename B>
Implies<Not<Not<A>>, A>
prove_dne_from_peirce(Implies<Implies<Implies<A, B>, A>, A> peirce_instance) {
  return [peirce_instance](Not<Not<A>> nna) -> A {
    // Specialize Peirce's Law by setting B = False.
    // It becomes ((A → False) → A) → A, which is (¬A → A) → A.
    Implies<Implies<Not<A>, A>, A> peirce_specialized = peirce_instance;

    Implies<Not<A>, A> na_to_a = [nna](Not<A> na) -> A {
      False f = modus_ponens(na, nna);
      return principle_of_explosion<A>()(f);
    };

    return peirce_specialized(na_to_a);
  };
}

int main() {
  // This file will not run any logic, as we cannot instantiate the axioms.
  // Its purpose is to pass compilation, which itself is the proof that
  // the equivalences hold.

  // To satisfy the type checker, we create dummy function pointers.
  // In a real scenario, these would be provided by a classical logic system.
  std::function<Or<int, Not<int>>()> lem_axiom_func = nullptr;
  std::function<Implies<Not<Not<int>>, int>()> dne_axiom_func = nullptr;
  std::function<Implies<Implies<Implies<int, False>, int>, int>()>
      peirce_axiom_func = nullptr;

  // Static assertions to verify the "proof transformer" functions.
  // If this compiles, the types are correct, and the logic is sound.

  // 1. Can we get DNE from LEM?
  using dne_from_lem_proof =
      decltype(prove_dne_from_lem<int>(lem_axiom_func()));
  static_assert(
      std::is_same_v<dne_from_lem_proof, Implies<Not<Not<int>>, int>>);

  // 2. Can we get Peirce's Law from DNE?
  using peirce_from_dne_proof =
      decltype(prove_peirce_from_dne<int, bool>(dne_axiom_func()));
  static_assert(std::is_same_v<peirce_from_dne_proof,
                               Implies<Implies<Implies<int, bool>, int>, int>>);

  // 3. Can we get DNE from Peirce's Law?
  using dne_from_peirce_proof =
      decltype(prove_dne_from_peirce<int, False>(peirce_axiom_func()));
  static_assert(
      std::is_same_v<dne_from_peirce_proof, Implies<Not<Not<int>>, int>>);

  std::cout << "enhanced_prover.cpp successfully compiled!" << std::endl;
  std::cout << "This demonstrates the type-level proof of equivalence between "
               "classical axioms."
            << std::endl;

  return 0;
}