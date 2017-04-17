#include <iostream>
#include <assert.h>
#include "smart_enum.h"

SMART_ENUM(Animal, int) {
    SM_ENUM_ELEM(Dog,    1,  "dog"  , "dog_description")
    SM_ENUM_ELEM(Cat,    2,  "cat"  )
    SM_ENUM_ELEM(Lion,   5)
    SM_ENUM_ELEM(Horse,  10,  "hOrse")
};

int main()
{

    Animal pet_cat   = Animal::Cat;
    Animal pet_dog   = Animal::Dog;
    Animal pet_lion  = Animal::Lion;
    Animal pet_horse = Animal::Horse;

    /* to string conversion */
    std::cout << "pet name: " << smart_enum::to_string(pet_cat) << std::endl;
    //pet name: cat
    std::cout << "pet name: " << smart_enum::to_string(pet_lion) << std::endl;
    //pet name: Lion


    /* It is possible to get enum by name */
    constexpr Animal ct_pet_lion = Animal::from_string("Lion");


    /* get description for smart enum */
    std::cout << "descr.: " << pet_dog.description() << std::endl;
    //descr.: dog_description
    std::cout << "descr.: " << pet_horse.description() << std::endl;
    //descr.: hOrse


    /* usage in case statements */
    switch (pet_cat) {
        case Animal::Cat:
            //do_something
            break;
        case Animal::Horse:
            //do_something
            break;
        default:
            //do_something
            break;
    }

    /* get number of declared elements in smart enum */
    std::cout << "Elements in smart enum: " << smart_enum::enum_size<Animal>() << std::endl;
    //Elements in smart enum: 4

    /* It is possible to check if data in smart enum are valid */
    Animal invalid(Animal::Cat);
    *(reinterpret_cast<int*>(&invalid)) = 6666;
    assert(invalid.check() == false);

}
