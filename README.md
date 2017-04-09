# Smart enum
[![Build Status](https://travis-ci.org/seleznevae/smart_enum.svg?branch=master)](https://travis-ci.org/seleznevae/smart_enum)
- [Design goals](#design-goals)
- [Examples](#examples)
- [License](#license)

Smart enum is a short library for smart enums in C++.

## Design goals

C++ doesn't have reflection. It is a common technique to use macros to add compile time information about enumeration like the following:

```c++
enum class Animal {
  Dog = 1,
  Cat = 1,
  Lion = 1,
  Horse = 1,
};

DECLARE_ENUM(Animal, Animal::Dog, Animal::Cat, Animal::Lion, Animal::Horse);
```
This code violates **DRY** (don't repeat yourself) principle. The aim of **smart enum** library is to provide a user with macros that can be used in enum declaration and the user will not need to repeat enum elements.


## Examples

Declaration of smart enum looks like this:
```c++
#include "smart_enum.h"

SMART_ENUM(Animal, int) {
    SM_ENUM_ELEM(Dog,    1,  "dog"  , "dog_description");
    SM_ENUM_ELEM(Cat,    2,  "cat"  );
    SM_ENUM_ELEM(Lion,   5);
    SM_ENUM_ELEM(Horse,  10,  "hOrse");
};
```
Definitions of variables:
```c++
Animal pet_cat   = Animal::Cat;
Animal pet_dog   = Animal::Dog;
Animal pet_lion  = Animal::Lion;
Animal pet_horse = Animal::Horse;
```

It is possible to convert smart enums to strings:
```c++
std::cout << "pet name: " << smart_enum::to_string(pet_cat) << std::endl;
//pet name: cat
std::cout << "pet name: " << smart_enum::to_string(pet_lion) << std::endl;
//pet name: Lion
```

It is possible get descriptions for smart enum values:
```c++
std::cout << "descr.: " << smart_enum::get_description(pet_dog) << std::endl;
//descr.: dog_description
std::cout << "descr.: " << smart_enum::get_description(pet_horse) << std::endl;
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
std::cout << "Elements in smart enum: " << smart_enum::enum_size<Animal>() << std::endl;
//Elements in smart enum: 4
```

It is possible to get information about number of elements declared in smart enum:
```c++
Animal invalid;
*(reinterpret_cast<int*>(&invalid)) = 6666;
assert(smart_enum::check(invalid) == false);
```
## License
WTFPL 
