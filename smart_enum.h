/*
                          _
 ___ _ __ ___   __ _ _ __| |_    ___ _ __  _   _ _ __ ___
/ __| '_ ` _ \ / _` | '__| __|  / _ \ '_ \| | | | '_ ` _ \
\__ \ | | | | | (_| | |  | |_  |  __/ | | | |_| | | | | | |
|___/_| |_| |_|\__,_|_|   \__|  \___|_| |_|\__,_|_| |_| |_|

Smart Enum
Version 0.1.0
https://github.com/seleznevae/smart_enum

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2017 Anton Seleznev <seleznevae@protonmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SMART_ENUM_H
#define SMART_ENUM_H


#include <type_traits>
#include <array>

namespace smart_enum
{

namespace detail
{

template <template <int, int> class SpecClass, int k>
using is_specialized_for_first = std::is_default_constructible<SpecClass<k, 0>>;

template <typename EnumType, int k, bool is_specialized>
struct enum_sz_impl_class;

template <typename EnumType, int k >
struct enum_sz_impl_class<EnumType, k, true> {
    static constexpr size_t enum_sz() { return 1 + enum_sz_impl_class<EnumType,
                                                            k + 1,
                                                            is_specialized_for_first<EnumType:: template ClassToSpec, k + 1>::value
                                                            >::enum_sz(); }
};

template <typename EnumType, int k>
struct enum_sz_impl_class<EnumType, k, false> {
    static constexpr size_t enum_sz() { return 0; }
};

template <typename EnumType, int k>
constexpr size_t enum_size_impl()
{
    return enum_sz_impl_class<EnumType, k, is_specialized_for_first<EnumType:: template ClassToSpec, k>::value >::enum_sz();
}

} //detail

template <typename EnumType>
constexpr size_t enum_size()
{
    return detail::enum_size_impl<EnumType, EnumType::base_value + 1>();
}

///////////////////////////////////////////////////////////////////////////////

namespace detail
{
    template<class T>
    struct void_t {
      typedef void type;
    };
}



template<class T, class U = void>
struct is_smart_enum {
    enum { value = 0 };
};

template<class T>
struct is_smart_enum<T, typename smart_enum::detail::void_t<typename T::is_smart_enum>::type > {
    enum { value = 1 };
};


template <typename T>
constexpr bool is_smart_enum_v = smart_enum::is_smart_enum<T>::value;

////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{
    template <typename T, typename U, bool is_T_integral, bool is_T_smart_enum>
    struct enum_cast_impl {
        static_assert((smart_enum::is_smart_enum_v<U> || std::is_integral<T>::value)
                      && (smart_enum::is_smart_enum_v<T> || std::is_integral<U>::value),
                      "Argument and return value of smart_enum::enum_cast should be an integral or"
                      " a smart enum");
    };

    template <typename T, typename U>
    struct enum_cast_impl<T, U, false, true> {
        static_assert((smart_enum::is_smart_enum_v<U> || std::is_integral<U>::value)
                      && (smart_enum::is_smart_enum_v<T> || std::is_integral<T>::value),
                      "Argument and return value of smart_enum::enum_cast should be an integral or"
                      " a smart enum");
        static constexpr T enum_cast(U value, bool *ok = nullptr)
        {
            T result(typename T::InternHelpType{static_cast<typename T::value_t>(value)});
            if (ok)
                *ok = result.check();
            return result;
        }
    };

    template <typename T, typename U>
    struct enum_cast_impl<T, U, true, false> {
        static_assert((smart_enum::is_smart_enum_v<U> || std::is_integral<U>::value)
                      && (smart_enum::is_smart_enum_v<T> || std::is_integral<T>::value),
                      "Argument and return value of smart_enum::enum_cast should be an integral or"
                      " a smart enum");
        static constexpr T enum_cast(U value, bool *ok = nullptr)
        {
            if (ok)
                *ok = true;
            return static_cast<T>(value.enum_member.value);
        }
    };

}


template <typename T, typename U>
constexpr T enum_cast(U value, bool *ok = nullptr)
{
    return detail::enum_cast_impl<T, U,
                                    std::is_integral<T>::value,
                                    smart_enum::is_smart_enum_v<T>
                                 >::enum_cast(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename EnumType, int i, int k >
struct enum_check_impl_class {
    constexpr static bool check(EnumType enum_value)
    {
        return enum_value.enum_member == static_cast<typename EnumType::internal_enum_t>(EnumType::template ClassToSpec<EnumType::base_value+1+i,0>::value)
                ? true
                : detail::enum_check_impl_class<EnumType, i + 1, enum_size<EnumType>()>::check(enum_value);
    }
};

template <typename EnumType, int k >
struct enum_check_impl_class<EnumType, k, k> {
    constexpr static bool check(EnumType) { return false; }
};

} //detail

template <typename EnumType>
constexpr bool enum_check(EnumType enum_value)
{
    return detail::enum_check_impl_class<EnumType, 0, enum_size<EnumType>()>::check(enum_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
namespace detail
{

template <typename EnumType, int i, int k >
struct index_of_impl_class {
    constexpr static ssize_t index_of(EnumType enum_value)
    {
        return enum_value.enum_member == static_cast<typename EnumType::internal_enum_t>(EnumType::template ClassToSpec<EnumType::base_value+1+i,0>::value)
                ? i
                : detail::index_of_impl_class<EnumType, i + 1, enum_size<EnumType>()>::index_of(enum_value);
    }
};

template <typename EnumType, int k >
struct index_of_impl_class<EnumType, k, k> {
    constexpr static ssize_t index_of(EnumType) { return -1; }
};

} //detail

template <typename EnumType>
constexpr ssize_t index_of(EnumType value)
{
    return detail::index_of_impl_class<EnumType, 0, enum_size<EnumType>()>::index_of(value);
}

/////////////////////////////////////////////////////////////////////////////////////////////

template <typename EnumType, size_t n>
constexpr EnumType
get_n()
{
    static_assert(n < smart_enum::enum_size<EnumType>(), "Incorrect get argument");
    constexpr auto tmp = static_cast<typename EnumType::internal_enum_t>(EnumType::template ClassToSpec<EnumType::base_value+1+n,0>::value);
    return EnumType(typename EnumType::InternHelpType(tmp));
}

template <typename EnumType, size_t n>
constexpr const char*
get_name_n()
{
    static_assert(n < smart_enum::enum_size<EnumType>(), "Incorrect get argument");
    return EnumType::template ClassToSpec<EnumType::base_value+1+n,0>::name();
}

template <typename EnumType, size_t n>
constexpr const char*
get_description_n()
{
    static_assert(n < smart_enum::enum_size<EnumType>(), "Incorrect get argument");
    return EnumType::template ClassToSpec<EnumType::base_value+1+n,0>::description();
}

//////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

    namespace array_detail
    {
        template<long unsigned... Is> struct seq{};
        template<long unsigned N, long unsigned... Is>
        struct gen_seq : gen_seq<N-1, N-1, Is...>{};
        template<long unsigned... Is>
        struct gen_seq<0, Is...> : seq<Is...>{};

        template<typename T, long unsigned N1, long unsigned... I1, long unsigned N2, long unsigned... I2>
        // Expansion pack
        constexpr std::array<T, N1+N2> concat_impl(const std::array<T, N1>& a1, const std::array<T, N2>& a2, seq<I1...>, seq<I2...>){
          return { a1[I1]..., a2[I2]... };
        }

        template<typename T, long unsigned N1, long unsigned N2>
        constexpr std::array<T, N1+N2> concat(const std::array<T, N1>& a1, const std::array<T, N2>& a2)
        {
          return concat_impl(a1, a2, gen_seq<N1>{}, gen_seq<N2>{});
        }
    } //array_detail

    template <typename EnumType, int i, int k >
    struct values_impl_class {
    constexpr static std::array<EnumType, k - i>
    values()
    {
        return array_detail::concat(std::array<EnumType, 1> {smart_enum::get_n<EnumType, i>()},
                                    detail::values_impl_class<EnumType, i + 1, k>::values());
    }
    };

    template <typename EnumType, int k >
    struct values_impl_class<EnumType, k, k> {
        constexpr static std::array<EnumType, 0>
        values() { return {}; }
    };

} //detail

template <typename EnumType>
constexpr std::array<EnumType, smart_enum::enum_size<EnumType>()>
values()
{
    return detail::values_impl_class<EnumType, 0, enum_size<EnumType>()>::values();
}

//////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

    template <typename EnumType, int i, int k >
    struct names_impl_class {
    constexpr static std::array<const char*, k - i>
    names()
    {
        return array_detail::concat(std::array<const char*, 1> {smart_enum::get_name_n<EnumType, i>()},
                                    detail::names_impl_class<EnumType, i + 1, k>::names());
    }
};

    template <typename EnumType, int k >
    struct names_impl_class<EnumType, k, k> {
        constexpr static std::array<const char*, 0>
        names() { return {}; }
    };

}

template <typename EnumType>
constexpr std::array<const char*, smart_enum::enum_size<EnumType>()>
names()
{
    return detail::names_impl_class<EnumType, 0, enum_size<EnumType>()>::names();
}

//////////////////////////////////////////////////////////////////////////////////////

namespace detail
{

    template <typename EnumType, int i, int k >
    struct descriptions_impl_class {
        constexpr static std::array<const char*, k - i>
        descriptions()
        {
            return array_detail::concat(std::array<const char*, 1> {smart_enum::get_description_n<EnumType, i>()},
                                        detail::descriptions_impl_class<EnumType, i + 1, k>::descriptions());
        }
    };

    template <typename EnumType, int k >
    struct descriptions_impl_class<EnumType, k, k> {
        constexpr static std::array<const char*, 0>
        descriptions() { return {}; }
    };

}

template <typename EnumType>
constexpr std::array<const char*, smart_enum::enum_size<EnumType>()>
descriptions()
{
    return detail::descriptions_impl_class<EnumType, 0, enum_size<EnumType>()>::descriptions();
}


/////////////////////////////////////////////////////////////////////////////////
namespace detail
{
    template <typename SmartEnum, size_t CurPos, size_t Size>
    struct enum_to_string_impl {
        static constexpr
        const char* to_string(ssize_t pos_to_extract)
        {
            return CurPos == pos_to_extract
                    ? get_name_n<SmartEnum, CurPos>()
                    : enum_to_string_impl<SmartEnum, CurPos+1, Size>::to_string(pos_to_extract)
                ;
        }
    };

    template <typename SmartEnum, size_t Size>
    struct enum_to_string_impl<SmartEnum, Size, Size>
    {
        static constexpr
        const char* to_string(ssize_t /*pos_to_extract*/)
        {
            return "Unknown";
        }
    };
}

template <typename EnumType>
constexpr const char* to_string(EnumType enum_value, bool *ok = nullptr)
{
    ssize_t index = smart_enum::index_of(enum_value);
    if (index == -1) {
        return      ok
                ? (*ok = false, "Unknown")
                : (throw std::bad_alloc(), "Unknown");
    }
    if (ok)
        *ok = true;
    return detail::enum_to_string_impl< EnumType,
                                        0,
                                        smart_enum::enum_size<EnumType>()
                                        >::to_string(index);

}

/////////////////////////////////////////////////////////////////////////////////
namespace detail
{
    template <typename SmartEnum, size_t CurPos, size_t Size>
    struct enum_to_description_impl {
        static constexpr
        const char* description(ssize_t pos_to_extract)
        {
            return CurPos == pos_to_extract
                    ? get_description_n<SmartEnum, CurPos>()
                    : enum_to_description_impl<SmartEnum, CurPos+1, Size>::description(pos_to_extract)
                ;
        }
    };

    template <typename SmartEnum, size_t Size>
    struct enum_to_description_impl<SmartEnum, Size, Size>
    {
            static constexpr
            const char* description(ssize_t /*pos_to_extract*/)
            {
                return "Unknown";
            }
        };
}

template <typename EnumType>
constexpr const char* description(EnumType enum_value, bool *ok = nullptr)
{
    ssize_t index = smart_enum::index_of(enum_value);
    if (index == -1) {
        return      ok
                ? (*ok = false, "Unknown")
                : (throw std::bad_alloc(), "Unknown");
    }
    if (ok)
        *ok = true;
    return detail::enum_to_description_impl< EnumType,
                                        0,
                                        smart_enum::enum_size<EnumType>()
                                        >::description(index);

}



} //smart_enum



///////////////////////////////////////////////////////////////////////////////

namespace nms
{
struct some_struct {
template <typename T>
struct BBB_base {
  using underl_t = int;
  enum class internal_enum_t:underl_t {
  };
  using value_t = internal_enum_t;

  constexpr BBB_base(internal_enum_t member_): enum_member(member_) { }
  static constexpr int base_value = __COUNTER__;

  // constructor helper
  template <underl_t k>
  struct HelperClass {
      constexpr  HelperClass(internal_enum_t v): value(static_cast<underl_t>(v)) { }
//      constexpr  HelperClass(const HelperClass<k> &v): value(v.value) { }
      underl_t value;
      constexpr operator internal_enum_t() const { return static_cast<internal_enum_t>(value); }
      constexpr bool operator ==(HelperClass<k> rhs) { return this->value == rhs.value; }
      constexpr bool operator !=(HelperClass<k> rhs) { return this->value != rhs.value; }
  };

  using InternHelpType = HelperClass<1>;
  InternHelpType enum_member;
  using BaseType = BBB_base<T>;
  using is_smart_enum = std::true_type;

  static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }
  constexpr  operator internal_enum_t() const { return static_cast<internal_enum_t>(enum_member.value); }
  constexpr bool check() const { return smart_enum::enum_check<T>(static_cast<const T&>(*this)); }
  constexpr ssize_t index() const { return smart_enum::index_of<T>(static_cast<const T&>(*this)); }
  constexpr static auto  values() { return smart_enum::values<T>(); }
  constexpr static auto  names() { return smart_enum::names<T>(); }
  constexpr static auto  descriptions() { return smart_enum::descriptions<T>(); }
  constexpr const char* to_string(bool *ok = nullptr)const { return smart_enum::to_string<T>(static_cast<const T&>(*this), ok); }
  constexpr const char* description(bool *ok = nullptr)const { return smart_enum::description<T>(static_cast<const T&>(*this), ok); }
  template <typename U = underl_t>
  constexpr U to_integral() const { return smart_enum::enum_cast<U>(static_cast<const T&>(*this)); }
  template <typename U>
  static constexpr T from_integral(U integr_v, bool *ok = nullptr) { return smart_enum::enum_cast<T>(integr_v, ok); }
};

template <int k, int DummyInt>
struct SmartEnumMutualAlias222;

using BBB = SmartEnumMutualAlias222<__COUNTER__, 0>;
static constexpr size_t SmartEnumMutualAlias222_BBB1 = __COUNTER__ - 1;

template <int DummyInt>
struct SmartEnumMutualAlias222<SmartEnumMutualAlias222_BBB1, DummyInt>:
        public BBB_base<SmartEnumMutualAlias222<SmartEnumMutualAlias222_BBB1, 0>>{

    using BaseClass_enum_elem0 = BBB_base<SmartEnumMutualAlias222<SmartEnumMutualAlias222_BBB1, DummyInt>>;
    static constexpr InternHelpType enum_elem0 = static_cast<typename BaseClass_enum_elem0::internal_enum_t>(4);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr typename BaseClass_enum_elem0::underl_t AAA_counter_enum_elem0 = __COUNTER__ - 2;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem0, dummy> {
        static constexpr typename BaseClass_enum_elem0::underl_t value = static_cast<typename BaseClass_enum_elem0::underl_t>(enum_elem0.value);
        static constexpr auto name() { return "enum_elem0_name"; }
        static constexpr auto description() { return "enum_elem0_description"; }
    };
    template <int k>
    using HelperClass_enum_elem0 = typename BaseClass_enum_elem0::template HelperClass<k>;
    constexpr SmartEnumMutualAlias222(HelperClass_enum_elem0<AAA_counter_enum_elem0 - BaseClass_enum_elem0::base_value> v): BaseClass_enum_elem0(static_cast<typename BaseClass_enum_elem0::internal_enum_t>(v.value)) {}


    using BaseClass_enum_elem1 = BBB_base<SmartEnumMutualAlias222<SmartEnumMutualAlias222_BBB1, DummyInt>>;
    static constexpr typename BaseClass_enum_elem1::InternHelpType enum_elem1 = static_cast<typename BaseClass_enum_elem1::internal_enum_t>(9);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr  typename BaseClass_enum_elem1::underl_t AAA_counter_enum_elem1 = __COUNTER__ - 2;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem1, dummy> {
        static constexpr typename BaseClass_enum_elem1::underl_t value = static_cast<typename BaseClass_enum_elem1::underl_t>(enum_elem1.value);
        static constexpr auto name() { return "enum_elem1_name"; }
        static constexpr auto description() { return "enum_elem1_description"; }
    };
    template <int k>
    using HelperClass_enum_elem1 = typename BaseClass_enum_elem0::template HelperClass<k>;
    constexpr SmartEnumMutualAlias222(HelperClass_enum_elem1<AAA_counter_enum_elem1 - BaseClass_enum_elem1::base_value> v): BaseClass_enum_elem1(static_cast<typename BaseClass_enum_elem1::internal_enum_t>(v.value)) {}
};
};
}

//////////////////////////////////////////////////////////////////////////////
///
namespace aaaa{
struct TestClass2 {


template <typename T>
struct CCC_base {
  using underl_t = int;
  enum class internal_enum_t:underl_t {
  };
  using value_t = internal_enum_t;

  constexpr CCC_base(internal_enum_t member_): enum_member(member_) { }
  static constexpr auto base_value = __COUNTER__;

  // constructor helper
  template <underl_t k>
  struct HelperClass {
      constexpr  HelperClass(internal_enum_t v): value(static_cast<underl_t>(v)) { }
//      constexpr  HelperClass(const HelperClass<k> &v): value(v.value) { }
      underl_t value;
      constexpr operator internal_enum_t() const { return static_cast<internal_enum_t>(value); }
      constexpr bool operator ==(HelperClass<k> rhs) { return this->value == rhs.value; }
      constexpr bool operator !=(HelperClass<k> rhs) { return this->value != rhs.value; }
  };

  using InternHelpType = HelperClass<1>;
  InternHelpType enum_member;
  using BaseType = CCC_base<T>;
  using is_smart_enum = std::true_type;

  static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }
  constexpr  operator internal_enum_t() const { return static_cast<internal_enum_t>(enum_member.value); }
  constexpr bool check() const { return smart_enum::enum_check<T>(static_cast<const T&>(*this)); }
  constexpr ssize_t index() const { return smart_enum::index_of<T>(static_cast<const T&>(*this)); }
  constexpr static auto  values() { return smart_enum::values<T>(); }
  constexpr static auto  names() { return smart_enum::names<T>(); }
  constexpr static auto  descriptions() { return smart_enum::descriptions<T>(); }
  constexpr const char* to_string(bool *ok = nullptr)const { return smart_enum::to_string<T>(static_cast<const T&>(*this), ok); }
  constexpr const char* description(bool *ok = nullptr)const { return smart_enum::description<T>(static_cast<const T&>(*this), ok); }
  template <typename U = underl_t>
  constexpr U to_integral() const { return smart_enum::enum_cast<U>(static_cast<const T&>(*this)); }
  template <typename U>
  static constexpr T from_integral(U integr_v, bool *ok = nullptr) { return smart_enum::enum_cast<T>(integr_v, ok); }

};

template <int k, int DummyInt>
struct SmartEnumMutualAlias333;

using CCC = SmartEnumMutualAlias333<__COUNTER__, 0>;
static constexpr size_t SmartEnumMutualAlias222_CCC1 = __COUNTER__ - 1;

//template <int DummyInt>


template <int DummyInt>
struct SmartEnumMutualAlias333<SmartEnumMutualAlias222_CCC1, DummyInt>:
        public CCC_base<SmartEnumMutualAlias333<SmartEnumMutualAlias222_CCC1, 0>>{

    using BaseClass_enum_elem0 = CCC_base<SmartEnumMutualAlias333<base_value + 1, DummyInt>>;
    static constexpr InternHelpType enum_elem0 = static_cast<typename BaseClass_enum_elem0::internal_enum_t>(4);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr underl_t AAA_counter_enum_elem0 = __COUNTER__ - 2;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem0, dummy> {
        static constexpr underl_t value = static_cast<underl_t>(enum_elem0.value);
        static constexpr auto name() { return "enum_elem0_name"; }
        static constexpr auto description() { return "enum_elem0_description"; }
    };
    constexpr SmartEnumMutualAlias333(HelperClass<AAA_counter_enum_elem0 - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}
//    constexpr SmartEnumMutualAlias(BaseType b): BaseType(static_cast<internal_enum_t>(b.enum_member.value)) {}

    using BaseClass_enum_elem1 = CCC_base<SmartEnumMutualAlias333<base_value + 1, DummyInt>>;
    static constexpr InternHelpType enum_elem1 = static_cast<typename BaseClass_enum_elem1::internal_enum_t>(9);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr  underl_t AAA_counter_enum_elem1 = __COUNTER__ - 2;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem1, dummy> {
        static constexpr underl_t value = static_cast<underl_t>(enum_elem1.value);
        static constexpr auto name() { return "enum_elem1_name"; }
        static constexpr auto description() { return "enum_elem1_description"; }
    };
    constexpr SmartEnumMutualAlias333(HelperClass<AAA_counter_enum_elem1 - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}
};
};
}


//////////////////////////////////////////////////////////////////////////////
template <typename T>
struct AAA_base {
  using underl_t = int;
  enum class internal_enum_t:underl_t {
  };
  using value_t = internal_enum_t;

  constexpr AAA_base(internal_enum_t member_): enum_member(member_) { }
  static constexpr int base_value = __COUNTER__;

  // constructor helper
  template <underl_t k>
  struct HelperClass {
      constexpr  HelperClass(internal_enum_t v): value(static_cast<underl_t>(v)) { }
//      constexpr  HelperClass(const HelperClass<k> &v): value(v.value) { }
      underl_t value;
      constexpr operator internal_enum_t() const { return static_cast<internal_enum_t>(value); }
      constexpr bool operator ==(HelperClass<k> rhs) { return this->value == rhs.value; }
      constexpr bool operator !=(HelperClass<k> rhs) { return this->value != rhs.value; }
  };

  using InternHelpType = HelperClass<1>;
  InternHelpType enum_member;
  using BaseType = AAA_base<T>;
  using is_smart_enum = std::true_type;

  static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }
  constexpr  operator internal_enum_t() const { return static_cast<internal_enum_t>(enum_member.value); }
  constexpr bool check() const { return smart_enum::enum_check<T>(static_cast<const T&>(*this)); }
  constexpr ssize_t index() const { return smart_enum::index_of<T>(static_cast<const T&>(*this)); }
  constexpr static auto  values() { return smart_enum::values<T>(); }
  constexpr static auto  names() { return smart_enum::names<T>(); }
  constexpr static auto  descriptions() { return smart_enum::descriptions<T>(); }
  constexpr const char* to_string(bool *ok = nullptr)const { return smart_enum::to_string<T>(static_cast<const T&>(*this), ok); }
  constexpr const char* description(bool *ok = nullptr)const { return smart_enum::description<T>(static_cast<const T&>(*this), ok); }
  template <typename U = underl_t>
  constexpr U to_integral() const { return smart_enum::enum_cast<U>(static_cast<const T&>(*this)); }
  template <typename U>
  static constexpr T from_integral(U integr_v, bool *ok = nullptr) { return smart_enum::enum_cast<T>(integr_v, ok); }
};


template <int k>
struct SmartEnumMutualAlias;

using AAA = SmartEnumMutualAlias<__COUNTER__>;

template <>
struct SmartEnumMutualAlias<__COUNTER__ - 1>:
        public AAA_base<SmartEnumMutualAlias<__COUNTER__ - 2>>{

    static constexpr InternHelpType enum_elem0 = static_cast<internal_enum_t>(4);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr underl_t AAA_counter_enum_elem0 = __COUNTER__ - 3;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem0, dummy> {
        static constexpr underl_t value = static_cast<underl_t>(enum_elem0.value);
        static constexpr auto name() { return "enum_elem0_name"; }
        static constexpr auto description() { return "enum_elem0_description"; }
    };
    constexpr SmartEnumMutualAlias(HelperClass<AAA_counter_enum_elem0 - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}
//    constexpr SmartEnumMutualAlias(BaseType b): BaseType(static_cast<internal_enum_t>(b.enum_member.value)) {}

    static constexpr InternHelpType enum_elem1 = static_cast<internal_enum_t>(9);
    template <int, int dummy>
    struct ClassToSpec;
    static constexpr  underl_t AAA_counter_enum_elem1 = __COUNTER__ - 3;
    template <int dummy>
    struct ClassToSpec<AAA_counter_enum_elem1, dummy> {
        static constexpr underl_t value = static_cast<underl_t>(enum_elem1.value);
        static constexpr auto name() { return "enum_elem1_name"; }
        static constexpr auto description() { return "enum_elem1_description"; }
    };
    constexpr SmartEnumMutualAlias(HelperClass<AAA_counter_enum_elem1 - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}
};


#define SMART_ENUM(enum_name,enum_und_type) \
    template <typename T>\
    struct enum_name##_base {\
      using underl_t = enum_und_type;\
      enum class internal_enum_t:underl_t {\
      };\
      using value_t = internal_enum_t;\
\
      constexpr enum_name##_base(internal_enum_t member_): enum_member(member_) { }\
      static constexpr int base_value = __COUNTER__;\
\
      template <underl_t k>\
      struct HelperClass {\
          constexpr  HelperClass(internal_enum_t v): value(static_cast<underl_t>(v)) { }\
          underl_t value;\
          constexpr operator internal_enum_t() const { return static_cast<internal_enum_t>(value); }\
      };\
    \
      using InternHelpType = HelperClass<1>;\
      InternHelpType enum_member;\
      using BaseType = enum_name##_base<T>; \
      using is_smart_enum = std::true_type; \
    \
      static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }\
      constexpr  operator internal_enum_t() const { return static_cast<internal_enum_t>(enum_member.value); }\
      constexpr bool check() const { return smart_enum::enum_check<T>(static_cast<const T&>(*this)); }\
      constexpr ssize_t index() const { return smart_enum::index_of<T>(static_cast<const T&>(*this)); }\
      constexpr static auto  values() { return smart_enum::values<T>(); } \
      constexpr static auto  names()  { return smart_enum::names<T>(); }\
      constexpr static auto  descriptions() { return smart_enum::descriptions<T>(); }\
      constexpr const char* to_string(bool *ok = nullptr) const { return smart_enum::to_string<T>(static_cast<const T&>(*this), ok); }\
      constexpr const char* description(bool *ok = nullptr)const { return smart_enum::description<T>(static_cast<const T&>(*this), ok); }\
      template <typename U = underl_t> \
      constexpr U to_integral() const { return smart_enum::enum_cast<U>(static_cast<const T&>(*this)); } \
      template <typename U> \
      static constexpr T from_integral(U integr_v, bool *ok = nullptr) { return smart_enum::enum_cast<T>(integr_v, ok); } \
    };\
    \
template <int k> \
struct SmartEnumMutualAlias;\
\
using enum_name = SmartEnumMutualAlias<__COUNTER__>; \
\
template <> \
struct SmartEnumMutualAlias<__COUNTER__ - 1>: \
        public enum_name##_base<SmartEnumMutualAlias<__COUNTER__ - 2>>



#define SM_ENUM_ELEM4(elem_id, elem_value, elem_name, elem_description)\
\
    static constexpr InternHelpType elem_id = static_cast<value_t>(elem_value);\
    template <int, int dummy>\
    struct ClassToSpec;\
    static constexpr underl_t counter_enum_##elem_id = __COUNTER__ - 3;\
    template <int dummy>\
    struct ClassToSpec<counter_enum_##elem_id, dummy> {\
        static constexpr underl_t value = static_cast<underl_t>(elem_id.value);\
        static constexpr auto name() { return elem_name ; }\
        static constexpr auto description() { return elem_description ; }\
    };\
    constexpr SmartEnumMutualAlias(HelperClass<counter_enum_##elem_id - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}


#endif // SMART_ENUM_H
