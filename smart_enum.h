#ifndef SMART_ENUM_H
#define SMART_ENUM_H


#include <type_traits>


namespace smart_enum
{

namespace detail
{

template <template <int, int> class SpecClass, int k>
using is_specialized_for_first = std::is_default_constructible<SpecClass<k, 0>>;


//template <typename EnumType, int k>
//size_t enum_size_impl()
//{
//    if constexpr(is_specialized_for_first<EnumType:: template ClassToSpec, k>::value)
//        return 1 + enum_size_impl<EnumType, k + 1>();
//    else
//        return 0;
//}

template <typename EnumType, int k, bool is_specialized>//typename dummy = typename std::enable_if<is_specialized_for_first<EnumType:: template ClassToSpec, k>::value> >
struct enum_sz_impl_class;

template <typename EnumType, int k >
struct enum_sz_impl_class<EnumType, k, true> {
    static size_t enum_sz() { return 1 + enum_sz_impl_class<EnumType,
                                                            k + 1,
                                                            is_specialized_for_first<EnumType:: template ClassToSpec, k + 1>::value
                                                            >::enum_sz(); }
};

template <typename EnumType, int k>
struct enum_sz_impl_class<EnumType, k, false> {
    static size_t enum_sz() { return 0; }
};

template <typename EnumType, int k>
size_t enum_size_impl()
{
    return enum_sz_impl_class<EnumType, k, is_specialized_for_first<EnumType:: template ClassToSpec, k>::value >::enum_sz();
//    return 1 + enum_size_impl<EnumType, k + 1>();
}



} //detail

template <typename EnumType>
size_t enum_size()
{
    return detail::enum_size_impl<EnumType, EnumType::base_value + 1>();
}
} //smart_enum



template <typename T>
struct AAA_base {
  using underl_t = int;
  enum class internal_enum_t:underl_t {
  };


  constexpr AAA_base(internal_enum_t member_): enum_member(member_) { }
  static constexpr int base_value = __COUNTER__;

  // constructor helper
  template <underl_t k>
  struct HelperClass {
      constexpr  HelperClass(internal_enum_t v): value(static_cast<underl_t>(v)) { }
      constexpr  HelperClass(const HelperClass<k> &v): value(v.value) { }
      underl_t value;
      constexpr operator internal_enum_t() const { return static_cast<internal_enum_t>(value); }
  };

  using InternHelpType = HelperClass<1>;
  InternHelpType enum_member;
  using BaseType = AAA_base<T>;

  static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }
  constexpr  operator internal_enum_t() const { return static_cast<internal_enum_t>(enum_member.value); }
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
          constexpr operator underl_t() const { return static_cast<underl_t>(value); }\
      };\
    \
      using InternHelpType = HelperClass<1>;\
      InternHelpType enum_member;\
      using BaseType = enum_name##_base<T>; \
    \
      static constexpr size_t enum_size() { return smart_enum::enum_size<T>(); }\
      constexpr  operator underl_t() const { return static_cast<underl_t>(enum_member.value); }\
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
        static constexpr auto name() { return #elem_name ; }\
        static constexpr auto description() { return #elem_description ; }\
    };\
    constexpr SmartEnumMutualAlias(HelperClass<counter_enum_##elem_id - base_value> v): BaseType(static_cast<internal_enum_t>(v.value)) {}


#endif // SMART_ENUM_H
