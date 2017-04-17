
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


SMART_ENUM(Animal) {
     SM_ENUM_ELEM(Dog,    1,  "dog"  , "dog_description")
     SM_ENUM_ELEM(Cat,    2,  "cat"  , "cat_description")
     SM_ENUM_ELEM(Lion,   3,  "Lion" , "lion_description")
     SM_ENUM_ELEM(Horse,  5,  "horse", "horse_description")
     SM_ENUM_ELEM(Tiger, 10,  "tiger")
     SM_ENUM_ELEM(Monkey,20)
     SM_ENUM_ELEM(Cow)
};

SMART_ENUM(Animal2, unsigned short) {
     SM_ENUM_ELEM(Dog2,    88,  "dog"  , "dog_description")
     SM_ENUM_ELEM(Cat2,    99,  "cat"  , "cat_description")
};

TEST_CASE( "Base tests", "[base]" )
{
    //constructors testing
    Animal aa1(Animal::Dog);
    Animal aa2{Animal::Dog};
    Animal aa3 = Animal{Animal::Dog};
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

    //doesn't compile in c++14
//    AAA bb = AAA::enum_elem0;
//    REQUIRE ( bb == AAA::enum_elem0 );

    aa1 = Animal::Cat;
    REQUIRE ( aa1 != aa2 );

    bool is_animal_und_type_cor = std::is_same<Animal::underl_t, int>::value;
    REQUIRE ( is_animal_und_type_cor == true);
    bool is_animal2_und_type_cor = std::is_same<Animal2::underl_t, unsigned short>::value;
    REQUIRE ( is_animal2_und_type_cor == true);
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

//namespace and inside class tests
namespace test_nms
{
    SMART_ENUM(Color, int) {
         SM_ENUM_ELEM(Red ,    1,  "Red"  , "Red")
         SM_ENUM_ELEM(Blue,    2,  "Blue" , "Blue")
    };

    struct TestClass {
        SMART_ENUM(Color, int) {
             SM_ENUM_ELEM(Red ,    1,  "Red"  , "Red")
             SM_ENUM_ELEM(Blue,    2,  "Blue" , "Blue")
        };
    };
}

TEST_CASE("Namespace and inside class tests", "[namespace]")
{
    test_nms::Color c1(test_nms::Color::Red);
    test_nms::Color c2{test_nms::Color::Red};
    test_nms::Color c3 = {test_nms::Color::Red};
    test_nms::Color c4 = test_nms::Color::Red;

    (void)c1;
    (void)c2;
    (void)c3;
    (void)c4;

    test_nms::TestClass::Color cc1(test_nms::TestClass::Color::Red);
    test_nms::TestClass::Color cc2{test_nms::TestClass::Color::Red};
    test_nms::TestClass::Color cc3 = {test_nms::TestClass::Color::Red};
    test_nms::TestClass::Color cc4 = test_nms::TestClass::Color::Red;

    (void)cc1;
    (void)cc2;
    (void)cc3;
    (void)cc4;
}



TEST_CASE( "Size tests", "[size]" )
{
    // enum size testing
    REQUIRE( sizeof(Animal)      == sizeof(int) );
    REQUIRE( sizeof(Animal::Dog) == sizeof(int) );

    // number of element in enum testing
    REQUIRE( Animal::enum_size() == 7          );
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

    test_nms::TestClass::Color c1(test_nms::TestClass::Color::Blue);
    REQUIRE ( smart_enum::index_of(c1) == 1 );
    REQUIRE ( c1.index() == 1 );

    *(int*)&a1 = 666;
    REQUIRE ( smart_enum::index_of(a1) == -1 );
    REQUIRE ( a1.index() == -1 );
}

// values tests
TEST_CASE( "Values tests", "[values]" )
{
    constexpr auto values = smart_enum::values<Animal>();
    std::array<Animal, 7> correct_values = {Animal::Dog,
                                            Animal::Cat,
                                            Animal::Lion,
                                            Animal::Horse,
                                            Animal::Tiger,
                                            Animal::Monkey,
                                            Animal::Cow};

    REQUIRE ( values           == correct_values );
    REQUIRE ( Animal::values() == correct_values );

    constexpr auto color_values = smart_enum::values<test_nms::TestClass::Color>();
    std::array<test_nms::TestClass::Color, 2> correct_color_values = {
                                            test_nms::TestClass::Color::Red,
                                            test_nms::TestClass::Color::Blue,
                                            };

    REQUIRE ( color_values                         == correct_color_values );
    REQUIRE ( test_nms::TestClass::Color::values() == correct_color_values );
}


TEST_CASE( "Names tests", "[namees]" )
{
    constexpr auto names = smart_enum::names<Animal>();
    std::array<std::string, 7> str_names;
    for (size_t i = 0; i < names.size(); ++i)
        str_names[i] = names[i];
    std::array<std::string, 7> correct_names = {"dog", "cat", "Lion", "horse", "tiger", "Monkey", "Cow"};
    REQUIRE ( str_names == correct_names );


    constexpr auto names2 = Animal::names();
    for (size_t i = 0; i < names2.size(); ++i)
        str_names[i] = names2[i];
    REQUIRE ( str_names == correct_names );

    constexpr auto color_names = smart_enum::names<test_nms::TestClass::Color>();
    std::array<std::string, 2> str_color_names;
    for (size_t i = 0; i < color_names.size(); ++i)
        str_color_names[i] = color_names[i];
    std::array<std::string, 2> correct_color_names = {"Red", "Blue"};
    REQUIRE ( str_color_names == correct_color_names );
}

TEST_CASE( "descriptions tests", "[descriptions]" )
{
    constexpr auto descriptions = smart_enum::descriptions<Animal>();
    std::array<std::string, 7> str_descriptions;
    for (size_t i = 0; i < descriptions.size(); ++i)
        str_descriptions[i] = descriptions[i];
    std::array<std::string, 7> correct_descriptions = {"dog_description",
                                                       "cat_description",
                                                       "lion_description",
                                                       "horse_description",
                                                       "tiger",
                                                       "Monkey",
                                                       "Cow"};

    REQUIRE ( str_descriptions == correct_descriptions );

    constexpr auto descriptions2 = Animal::descriptions();
    for (size_t i = 0; i < descriptions2.size(); ++i)
        str_descriptions[i] = descriptions2[i];
    REQUIRE ( str_descriptions == correct_descriptions );

    constexpr auto color_descriptions = smart_enum::descriptions<test_nms::TestClass::Color>();
    std::array<std::string, 2> str_color_descriptions;
    for (size_t i = 0; i < color_descriptions.size(); ++i)
        str_color_descriptions[i] = color_descriptions[i];
    std::array<std::string, 2> correct_color_descriptions = {"Red", "Blue"};
    REQUIRE ( str_color_descriptions == correct_color_descriptions );
}

TEST_CASE( "Test string representation of smart enums", "[to_string]" )
{
    constexpr Animal a1(Animal::Dog);
    constexpr Animal a2(Animal::Cat);
    constexpr Animal a3(Animal::Lion);
    constexpr Animal a4(Animal::Horse);

    constexpr const char* n1 = smart_enum::to_string(a1);
    constexpr const char* n2 = smart_enum::to_string(a2);
    constexpr const char* n3 = smart_enum::to_string(a3);
    constexpr const char* n4 = smart_enum::to_string(a4);

    REQUIRE( n1 == std::string("dog") );
    REQUIRE( n2 == std::string("cat") );
    REQUIRE( n3 == std::string("Lion") );
    REQUIRE( n4 == std::string("horse") );

    Animal invalid(Animal::Dog);
    *(int*)&invalid = 666;
    REQUIRE_THROWS_AS( smart_enum::to_string(invalid), std::bad_alloc );
    bool is_conversion_ok = true;
    smart_enum::to_string(invalid, &is_conversion_ok);
    REQUIRE(is_conversion_ok == false);
    invalid = Animal::Dog;
    smart_enum::to_string(invalid, &is_conversion_ok);
    REQUIRE(is_conversion_ok == true);

    REQUIRE( a1.to_string() == std::string("dog") );
    REQUIRE( a2.to_string() == std::string("cat") );
    REQUIRE( a3.to_string() == std::string("Lion") );
    REQUIRE( a4.to_string() == std::string("horse") );


    constexpr const char* col1 = smart_enum::to_string(test_nms::TestClass::Color(test_nms::TestClass::Color::Red));
    REQUIRE( col1 == std::string("Red") );
    constexpr const char* col2 = smart_enum::to_string(test_nms::TestClass::Color(test_nms::TestClass::Color::Blue));
    REQUIRE( col2 == std::string("Blue") );
}

TEST_CASE( "Test from string", "[from_string]" )
{
    constexpr Animal a1(Animal::Dog);
    constexpr Animal a2(Animal::Cat);
    constexpr Animal a3(Animal::Lion);
    constexpr Animal a4(Animal::Horse);

    constexpr Animal t1 = smart_enum::from_string<Animal>("dog");
    constexpr Animal t2 = smart_enum::from_string<Animal>("cat");
    constexpr Animal t3 = smart_enum::from_string<Animal>("Lion");
    constexpr Animal t4 = smart_enum::from_string<Animal>("horse");

    REQUIRE( a1 == t1 );
    REQUIRE( a2 == t2 );
    REQUIRE( a3 == t3 );
    REQUIRE( a4 == t4 );

    // doesn't compile as it should be
//    constexpr Animal t5 = smart_enum::from_string<Animal>("something");

    REQUIRE_THROWS_AS( smart_enum::from_string<Animal>("something"), std::bad_alloc );

    bool ok = true;
    smart_enum::from_string<Animal>("something", &ok);
    REQUIRE( ok == false );
    smart_enum::from_string<Animal>("Lion", &ok);
    REQUIRE( ok == true );


    constexpr Animal t11 = Animal::from_string("dog");
    constexpr Animal t12 = Animal::from_string("cat");
    constexpr Animal t13 = Animal::from_string("Lion");
    constexpr Animal t14 = Animal::from_string("horse");

    REQUIRE( a1 == t11 );
    REQUIRE( a2 == t12 );
    REQUIRE( a3 == t13 );
    REQUIRE( a4 == t14 );
}



TEST_CASE( "Test desription of smart enums", "[description]" )
{
    constexpr Animal a1(Animal::Dog);
    constexpr Animal a2(Animal::Cat);
    constexpr Animal a3(Animal::Lion);
    constexpr Animal a4(Animal::Horse);

    constexpr const char* n1 = smart_enum::description(a1);
    constexpr const char* n2 = smart_enum::description(a2);
    constexpr const char* n3 = smart_enum::description(a3);
    constexpr const char* n4 = smart_enum::description(a4);

    REQUIRE( n1 == std::string("dog_description") );
    REQUIRE( n2 == std::string("cat_description") );
    REQUIRE( n3 == std::string("lion_description") );
    REQUIRE( n4 == std::string("horse_description") );

    Animal invalid(Animal::Dog);
    *(int*)&invalid = 666;
    REQUIRE_THROWS_AS( smart_enum::description(invalid), std::bad_alloc );
    bool is_conversion_ok = true;
    smart_enum::description(invalid, &is_conversion_ok);
    REQUIRE(is_conversion_ok == false);
    invalid = Animal::Dog;
    smart_enum::description(invalid, &is_conversion_ok);
    REQUIRE(is_conversion_ok == true);

    REQUIRE( a1.description() == std::string("dog_description") );
    REQUIRE( a2.description() == std::string("cat_description") );
    REQUIRE( a3.description() == std::string("lion_description") );
    REQUIRE( a4.description() == std::string("horse_description") );

    constexpr const char* col_desc_1 = smart_enum::description(test_nms::TestClass::Color(test_nms::TestClass::Color::Red));
    REQUIRE( col_desc_1 == std::string("Red") );
    constexpr const char* col_desc_2 = smart_enum::description(test_nms::TestClass::Color(test_nms::TestClass::Color::Blue));
    REQUIRE( col_desc_2 == std::string("Blue") );
}

TEST_CASE( "Test is_smart_enum type_trait", "[is_smart_enum]" )
{
    REQUIRE( smart_enum::is_smart_enum<Animal>::value == true);
    REQUIRE( smart_enum::is_smart_enum_v<Animal> == true);

    REQUIRE( smart_enum::is_smart_enum<bool>::value == false);
    REQUIRE( smart_enum::is_smart_enum_v<bool> == false);

    enum Dummy {
        d1, d2
    };
    REQUIRE( smart_enum::is_smart_enum<Dummy>::value == false);
    REQUIRE( smart_enum::is_smart_enum_v<Dummy> == false);


    REQUIRE( smart_enum::is_smart_enum<test_nms::TestClass::Color>::value == true);
}

TEST_CASE( "Test enum_cast", "[enum_cast]" )
{
    Animal a1 = smart_enum::enum_cast<Animal>(1);
    Animal a2 = smart_enum::enum_cast<Animal>(2);
    Animal a3 = smart_enum::enum_cast<Animal>(3);
    Animal a4 = smart_enum::enum_cast<Animal>(5);

    REQUIRE( a1 == Animal(Animal::Dog));
    REQUIRE( a2 == Animal(Animal::Cat));
    REQUIRE( a3 == Animal(Animal::Lion));
    REQUIRE( a4 == Animal(Animal::Horse));

    int  i = smart_enum::enum_cast<int>(Animal(Animal::Horse));
    long l = smart_enum::enum_cast<long>(Animal(Animal::Horse));
    char c = smart_enum::enum_cast<char>(Animal(Animal::Horse));
    int  i2= smart_enum::enum_cast<int>(Animal(Animal::Cow));

    REQUIRE( i == 5 );
    REQUIRE( l == 5 );
    REQUIRE( c == 5 );
    REQUIRE( i2== 21);

    REQUIRE( a1.to_integral()           ==  1 );
    REQUIRE( a2.to_integral<uint64_t>() ==  2 );
    REQUIRE( a3.to_integral<char>()     ==  3 );
    REQUIRE( a4.to_integral<short>()    ==  5 );

    REQUIRE( a1 == Animal::from_integral(1) );
    REQUIRE( a2 == Animal::from_integral(2) );
    REQUIRE( a3 == Animal::from_integral(3) );
    REQUIRE( a4 == Animal::from_integral(5) );

    bool is_ok = true;
    Animal::from_integral(5, &is_ok);
    REQUIRE( is_ok == true );
    Animal::from_integral(25, &is_ok);
    REQUIRE( is_ok == false );

    //constexpr checking
    constexpr short sh = Animal(Animal::Dog).to_integral();
    REQUIRE (sh == 1);
    constexpr Animal an_dog = Animal::from_integral(1);
    REQUIRE (an_dog == a1);

    test_nms::TestClass::Color col1(test_nms::TestClass::Color::Blue);
    REQUIRE (2 == col1.to_integral());


    test_nms::test_class::CCC col2(test_nms::test_class::CCC::enum_elem2);
    REQUIRE (10 == col2.to_integral());

    // doesn't compile as it should be
//    auto  invalid1 = smart_enum::enum_cast<double>(Animal(Animal::Horse));
//    auto  invalid2 = smart_enum::enum_cast<Animal>(1.2);
}

