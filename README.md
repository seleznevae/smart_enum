# Smart enum
[![Build Status](https://travis-ci.org/seleznevae/smart_enum.svg?branch=master)](https://travis-ci.org/seleznevae/smart_enum)
 [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
- [Design goals](#design-goals)
- [Examples](#examples)
- [License](#license)

Header-only library for compile-time C++ enum introspection.

## Design goals

C++ doesn't have reflection. It is a common technique to use macros to add compile time information about enumeration like the following:

```c++
enum class Animal {
  Dog = 1,
  Cat = 2,
  Lion = 4,
  Horse = 5,
};

DECLARE_ENUM(Animal, Animal::Dog, Animal::Cat, Animal::Lion, Animal::Horse);
```
This code violates **DRY** (don't repeat yourself) principle. The aim of **smart enum** library is to provide a user with macros that can be used in enum declaration and the user will not need to repeat enum elements.

## Highlights

1. Smart enums can be declared in namespaces and classes.
2. Smart enum to and from string conversions.


## Examples

Declaration of smart enum looks like this:
```c++
#include "smart_enum.h"

SMART_ENUM(Animal, int) {
    SM_ENUM_ELEM(Dog,    1,  "dog"  , "dog_description");
    SM_ENUM_ELEM(Cat,    2,  "cat"  );
    SM_ENUM_ELEM(Lion,   5);
    SM_ENUM_ELEM(Horse);
};
```

For each enum constant string representation and string description is generated. When macro with 1 or 2 arguments is used
( like SM_ENUM_ELEM(Lion,   5) and SM_ENUM_ELEM(Horse) ) string representation and description will match enum constant name.
If you want to specify custom string representation and string description macro with 3 or 4 arguments should be used.

Definitions of variables:
```c++
Animal pet_cat   ={Animal::Cat};
Animal pet_dog   = Animal::Dog;
Animal pet_lion   (Animal::Lion);
Animal pet_horse  {Animal::Horse};
```

It is possible to convert smart enums to strings:
```c++
std::cout << "pet name: " << pet_cat.to_string() << std::endl;
//pet name: cat
std::cout << "pet name: " << pet_lion.to_string() << std::endl;
//pet name: Lion
```

It is possible get descriptions for smart enum values:
```c++
std::cout << "descr.: " << pet_dog.description() << std::endl;
//descr.: dog_description
std::cout << "descr.: " << pet_horse.description() << std::endl;
//descr.: hOrse
```

Smart enums can be used in switch statements like ordinal enums:
```c++
switch (pet_cat) {
     case Animal::Cat:
         assert(true);
         break;
     case Animal::Horse:
         assert(false);
         break;
     default:
         assert(false);
         break;
 }
```
It is possible to get information about number of elements declared in smart enum:
```c++
std::cout << "Elements in smart enum: " << Animal::size() << std::endl;
//Elements in smart enum: 4
```

It is possible to get all values, names and descriptions of smart enum:
```c++
std::cout << "smart enum names: ";
for (const auto &enum_item : Animal::values())
    std::cout << enum_item.to_string() << " ";
std::cout << std::endl;
//smart enum names: dog cat Lion Horse

std::cout << "smart enum names: ";
for (const auto &name : Animal::names())
    std::cout << name << " ";
std::cout << std::endl;
//smart enum names: dog cat Lion Horse

std::cout << "smart enum descriptions: ";
for (const auto &name : Animal::descriptions())
    std::cout << name << " ";
std::cout << std::endl;
//smart enum descriptions: dog_description cat Lion Horse



```

It is possible to check if smart enum value is valid:
```c++
Animal invalid;
*(reinterpret_cast<int*>(&invalid)) = 6666;
assert(invalid.is_valid() == false);
```

## Installation

Include smart_enum.h in your source code.

## License
The MIT license
