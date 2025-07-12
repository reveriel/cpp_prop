# cpp 命题逻辑

用 C++ 类型推导能力实现命题逻辑演算 (propositional logic).

两种方法

1.`template.h`  用模板的类型实现 propositional logic :

* 编译期的 propositional logic 计算器，需要枚举所有命题的真值组合，才能证明定理.

2.`prover.cpp`  用  c++ 函数的类型推导能力实现 propositional logic 的正确性验证,  展示 Curry-Howard 同构的思想: 命题即类型，证明即程序.

* 定理被编码为 C++ 的类型。
* 证明是这些类型的实例 (值或函数)。
* C++ 编译器通过类型检查来验证证明的正确性。
* 用 True{} 调用函数是在运行一个已经被编译时验证过的证明，以一个"真"的公理作为起点。
