# Smart enum
Smart enum - short test library for smart enums in C++.
Implementation is based on materials for [SMETA](http://b.atch.se/posts/constexpr-meta-container/attachments/stateful_meta_container-poc.cpp) (stateful meta-programming library) by Filip Roséen  	filip.roseen@gmail.com.

Author doesn't know if these techniques are valid according to the C++ standard and this programs are not ill-formed.
So **it's not recommended to use this library in your production code**. **This library is just an experiment**.

## Design goals

C++ doesn't have reflection. It is a common technique to use macros to add compile time inforamtion about enumeration like the following:

```c++
enum class Animal {
  Dog = 1,
  Cat = 1,
  Lion = 1,
  Horse = 1,
};

DECLARE_ENUM(Animal, Animal::Dog, Animal::Cat, Animal::Lion, Animal::Horse);
```
This code violates **DRY** (don't repeat yourself) principle. The aim of **smart enum** library to give macros that can be use in enum
declaration and user will not need to repeat enum elements.
