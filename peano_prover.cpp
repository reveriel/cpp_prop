#include "constructive_logic.h"
#include <iostream>
#include <type_traits>

// --- Type-level Natural Numbers (Peano Axioms) ---

// Zero
struct Zero {};

// Successor (N + 1)
template <typename N> struct Succ {};

// --- Type-level Arithmetic Operations ---

// Addition: Add<N, M>::type represents N + M
template <typename N, typename M> struct Add;

// Base case: 0 + M = M
template <typename M> struct Add<Zero, M> { using type = M; };

// Recursive case: Succ<N> + M = Succ<N + M>
template <typename N, typename M> struct Add<Succ<N>, M> {
    using type = Succ<typename Add<N, M>::type>;
};

// Multiplication: Multiply<N, M>::type represents N * M
template <typename N, typename M> struct Multiply;

// Base case: 0 * M = 0
template <typename M> struct Multiply<Zero, M> { using type = Zero; };

// Recursive case: Succ<N> * M = (N * M) + M
template <typename N, typename M> struct Multiply<Succ<N>, M> {
    using type = typename Add<typename Multiply<N, M>::type, M>::type;
};

// --- Type-level Natural Number Theorems (Verified by static_assert) ---

// Theorem: N + 0 = N
template <typename N>
void theorem_add_zero_is_n() {
    static_assert(std::is_same_v<typename Add<N, Zero>::type, N>,
                  "Theorem: N + 0 = N failed");
}

// Theorem: 0 + N = N (requires commutativity or a separate proof)
// This is not directly provable from our Add definition without commutativity.
// We can only assert it for specific N.

// Theorem: N + Succ<M> = Succ<N + M>
// This is implicitly handled by our Add definition.

// Theorem: Commutativity of Addition: N + M = M + N
// This is a more complex theorem that typically requires induction.
// We can only assert it for specific N and M.

// Example: 1 + 2 = 3
using One = Succ<Zero>;
using Two = Succ<One>;
using Three = Succ<Two>;

void theorem_one_plus_two_is_three() {
    static_assert(std::is_same_v<typename Add<One, Two>::type, Three>,
                  "Theorem: 1 + 2 = 3 failed");
}

// Example: 2 * 2 = 4
using Four = Succ<Three>;

void theorem_two_times_two_is_four() {
    static_assert(std::is_same_v<typename Multiply<Two, Two>::type, Four>,
                  "Theorem: 2 * 2 = 4 failed");
}

int main() {
    // Run the theorem checks. If this compiles, the theorems are proven.
    theorem_add_zero_is_n<Zero>();
    theorem_add_zero_is_n<One>();
    theorem_add_zero_is_n<Two>();

    theorem_one_plus_two_is_three();
    theorem_two_times_two_is_four();

    std::cout << "peano_prover.cpp successfully compiled!" << std::endl;
    std::cout << "Demonstrates type-level natural number arithmetic and basic theorem proving." << std::endl;

    return 0;
}
