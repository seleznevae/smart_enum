
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <assert.h>
#include <cstddef>
#include <utility>

#include "smart_enum.h"
#include "third_party/Catch/catch.hpp"

using namespace std;


SMART_ENUM(Animal, int) {
     SM_ENUM_ELEM4(Dog,    1,  "dog"  , "dog_description")
     SM_ENUM_ELEM4(Cat,    2,  "cat"  , "cat_description")
     SM_ENUM_ELEM4(Lion,   3,  "Lion" , "lion_description")
     SM_ENUM_ELEM4(Horse,  5,  "horse", "horse_description")
};



using namespace smart_enum;
TEST_CASE( "Base tests", "[base]" )
{
    //constructors testing
    Animal aa1(Animal::Dog);
    Animal aa2{Animal::Dog};
    Animal aa3 = {Animal::Dog};
    Animal aa4 = Animal(Animal::Dog);

    (void)aa1;
    (void)aa2;
    (void)aa3;
    (void)aa4;

    // assignment operators testing
    aa1 = aa2;
    aa1 = Animal::Dog;

    //tests for equality operators
    REQUIRE ( aa1 == aa2 );
    aa1 = Animal::Cat;
    REQUIRE ( aa1 != aa2 );
}


TEST_CASE( "Switch-case tests", "[switch]")
{
    Animal aa(Animal::Cat);
    bool is_result_ok = false;
    switch (aa) {
        case Animal::Cat:
            is_result_ok = true;
            break;
        case Animal::Dog:
            is_result_ok = false;
            break;
        default:
            break;
    }

    REQUIRE (is_result_ok == true);
}

//namespace tests
namespace test_nms
{
    SMART_ENUM(Color, int) {
         SM_ENUM_ELEM4(Red ,    1,  "Red"  , "Red")
         SM_ENUM_ELEM4(Blue,    2,  "Blue" , "Blue")
    };
}

TEST_CASE("Namespace tests", "[namespace]")
{
    test_nms::Color c1(test_nms::Color::Red);
    test_nms::Color c2{test_nms::Color::Red};
    test_nms::Color c3 = {test_nms::Color::Red};
    test_nms::Color c4 = test_nms::Color::Red;

    (void)c1;
    (void)c2;
    (void)c3;
    (void)c4;
}

TEST_CASE( "Size tests", "[size]" )
{
    // enum size testing
    REQUIRE( sizeof(Animal)      == sizeof(int) );
    REQUIRE( sizeof(Animal::Dog) == sizeof(int) );

    // number of element in enum testing
    REQUIRE( Animal::enum_size() == 4          );
}


// check function tests
TEST_CASE( "Check tests", "[check]" )
{
    Animal a1(Animal::Dog);
    Animal a2(Animal::Cat);
    Animal a3(Animal::Lion);
    Animal a4(Animal::Horse);

    REQUIRE ( smart_enum::enum_check(a1) == true );
    REQUIRE ( smart_enum::enum_check(a2) == true );
    REQUIRE ( smart_enum::enum_check(a3) == true );
    REQUIRE ( smart_enum::enum_check(a4) == true );

    REQUIRE ( a1.check() == true );
    REQUIRE ( a2.check() == true );
    REQUIRE ( a3.check() == true );
    REQUIRE ( a4.check() == true );

    *(int*)&a1 = 0;
    *(int*)&a2 = 4;
    *(int*)&a3 = 6;
    REQUIRE ( smart_enum::enum_check(a1) == false );
    REQUIRE ( smart_enum::enum_check(a2) == false );
    REQUIRE ( smart_enum::enum_check(a3) == false );
    REQUIRE ( a1.check() == false );
    REQUIRE ( a2.check() == false );
    REQUIRE ( a3.check() == false );
}

enum class AAAAA {
    a,b
};

// index tests
TEST_CASE( "Index tests", "[index]" )
{
    Animal a1(Animal::Dog);
    Animal a2(Animal::Cat);
    Animal a3(Animal::Lion);
    Animal a4(Animal::Horse);

    REQUIRE ( smart_enum::index_of(a1) == 0 );
    REQUIRE ( smart_enum::index_of(a2) == 1 );
    REQUIRE ( smart_enum::index_of(a3) == 2 );
    REQUIRE ( smart_enum::index_of(a4) == 3 );

    REQUIRE ( a1.index() == 0 );
    REQUIRE ( a2.index() == 1 );
    REQUIRE ( a3.index() == 2 );
    REQUIRE ( a4.index() == 3 );

    *(int*)&a1 = 666;
    REQUIRE ( smart_enum::index_of(a1) == -1 );
    REQUIRE ( a1.index() == -1 );
}

// values tests
TEST_CASE( "Values tests", "[values]" )
{
    constexpr auto values = smart_enum::values<Animal>();
    std::array<Animal, 4> correct_values = {Animal::Dog, Animal::Cat, Animal::Lion, Animal::Horse};

    REQUIRE ( values           == correct_values );
    REQUIRE ( Animal::values() == correct_values );
}



/*TEST_CASE( "Base tests for smart enums", "[base]" )
{
    Animal pet_0 = Animal::Dog;
    Animal pet_1 = Animal::Horse;

    bool cond_0 = (pet_0 == Animal::Dog);
    bool cond_1 = (pet_1 == Animal::Horse);
    REQUIRE( cond_0 );
    REQUIRE( cond_1 );

    std::swap(pet_0, pet_1);
    cond_0 = (pet_0 == Animal::Horse);
    cond_1 = (pet_1 == Animal::Dog);
    REQUIRE( cond_0 );
    REQUIRE( cond_1 );
}


TEST_CASE( "Test descriptions of smart enums", "[description]" )
{
    REQUIRE( smart_enum::get_description(Animal(Animal::Dog))   == std::string("dog_description") );
    REQUIRE( smart_enum::get_description(Animal(Animal::Cat))   == std::string("cat") );
    REQUIRE( smart_enum::get_description(Animal(Animal::Lion))  == std::string("Lion") );
    REQUIRE( smart_enum::get_description(Animal(Animal::Horse)) == std::string("hOrse") );

    auto str_repr_0 = smart_enum::get_enum_description<0, Animal>();
    auto str_repr_1 = smart_enum::get_enum_description<1, Animal>();
    auto str_repr_2 = smart_enum::get_enum_description<2, Animal>();
    auto str_repr_3 = smart_enum::get_enum_description<3, Animal>();
    REQUIRE( str_repr_0 == std::string("dog_description") );
    REQUIRE( str_repr_1 == std::string("cat") );
    REQUIRE( str_repr_2 == std::string("Lion") );
    REQUIRE( str_repr_3 == std::string("hOrse") );
}

TEST_CASE( "Test string representation of smart enums", "[string]" )
{
    REQUIRE( smart_enum::to_string(Animal(Animal::Dog))   == std::string("dog") );
    REQUIRE( smart_enum::to_string(Animal(Animal::Cat))   == std::string("cat") );
    REQUIRE( smart_enum::to_string(Animal(Animal::Lion))  == std::string("Lion") );
    REQUIRE( smart_enum::to_string(Animal(Animal::Horse)) == std::string("hOrse") );

    //testing constexpr
    constexpr auto anim_0 = smart_enum::to_enum<Animal>("dog");
    constexpr auto anim_3 = smart_enum::to_enum<Animal>("hOrse");
    REQUIRE( Animal(Animal::Dog)   == anim_0 );
    REQUIRE( Animal(Animal::Horse) == anim_3 );

    constexpr auto anim_name_1 = smart_enum::to_string(Animal(Animal::Cat));
    constexpr auto anim_name_2 = smart_enum::to_string(Animal(Animal::Lion));
    REQUIRE( anim_name_1 == std::string("cat") );
    REQUIRE( anim_name_2 == std::string("Lion") );

    //    //won't compile as it should be
    //constexpr auto aa = smart_enum::to_string(smart_enum::int_to_enum_cast<Animal>(666));
    REQUIRE_THROWS_AS( smart_enum::to_string(smart_enum::int_to_enum_cast<Animal>(666)), std::bad_alloc );
}

TEST_CASE( "Test enum size evaluation", "[enum_size]" )
{
    REQUIRE( smart_enum::enum_size<Animal>()  == 4 );
}

TEST_CASE( "Test casts", "[cast]" )
{
    REQUIRE( static_cast<int>(Animal::Horse) == 10);
    Animal pet = Animal::Horse;
    REQUIRE( smart_enum::enum_to_int_cast<int>(pet) == 10);
    REQUIRE( smart_enum::int_to_enum_cast<Animal>(10) == pet);
}

TEST_CASE( "Checking data in enums", "[check]" )
{
    Animal valid(Animal::Horse);
    REQUIRE(smart_enum::check(valid) == true);

    Animal invalid;
    *(reinterpret_cast<int*>(&invalid)) = 6666;

    REQUIRE( smart_enum::check(invalid) == false);
    REQUIRE_THROWS_AS( smart_enum::to_string(invalid), std::bad_alloc);

    // Nonstrict conversion doesn't throw in invalid cases
    REQUIRE(smart_enum::to_string<smart_enum::NonStrictConvPolicy>(invalid) == std::string("Unknown"));
}


TEST_CASE( "Checking index of element in enums", "[index_of]" )
{
    REQUIRE(smart_enum::index_of<Animal>(Animal::Lion) == 2);
}*/
