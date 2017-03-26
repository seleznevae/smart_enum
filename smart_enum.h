#ifndef SMART_ENUM_H
#define SMART_ENUM_H


#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <assert.h>
#include <cstddef>
#include <utility>


#pragma GCC system_header  //trying supress warings

#pragma GCC diagnostic ignored "-Wnon-template-friend"



// smeta library source code
// author: Filip Roséen <filip.roseen@gmail.com>
// source: http://b.atch.se/posts/constexpr-meta-container

#define MAX_VALUE_READER 8

#ifndef ATCH_TYPE_LIST_HPP
#define ATCH_TYPE_LIST_HPP

namespace atch {
  template<std::size_t N>
  using ic = std::integral_constant<std::size_t, N>;

  template<std::size_t... Ns>
  using iseq = std::index_sequence<Ns...>;

  template<std::size_t N>
  using make_iseq = std::make_index_sequence<N>;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  template<class... Ts>
  struct type_list {
    using size_type = std::size_t;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    static constexpr size_type size () { return sizeof... (Ts); }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<size_type Idx>
    struct at {
      template<size_type N, class U, class... Us>
      struct access_helper
        : access_helper<N+1, Us...>
      { };

      template<class U, class... Us>
      struct access_helper<Idx, U, Us...> {
        using result = U;
      };

      using result = typename access_helper<0, Ts...>::result;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<class... Us>
    struct push {
      using result = type_list<Ts..., Us...>;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<size_type Idx, class U>
    struct set {

      template<size_type N>
      static auto at_helper (ic<N>)   -> typename at<N>::result;
      static auto at_helper (ic<Idx>) -> U;

      template<size_type... Ns>
      static auto set_helper (iseq<Ns...>)
        -> type_list<decltype (at_helper (ic<Ns> {}))...>
      ;

      using result = decltype (set_helper (make_iseq<size()> {}));
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    struct init {
      template<size_type... Ns>
      static auto helper (iseq<Ns...>)
        -> type_list<typename at<Ns>::result...>
      ;

      using result = decltype (helper (make_iseq<size ()-1> {}));
    };
  };

} /* namespace atch */


namespace atch { namespace {

  template<class Tag>
  struct meta_counter {
    using size_type = std::size_t;

    template<size_type N>
    struct ident {
      friend constexpr size_type adl_lookup (ident<N>);
      static constexpr size_type value = N;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<class Ident>
    struct writer {
      friend constexpr size_type adl_lookup (Ident) {
        return Ident::value;
      }

      static constexpr size_type value = Ident::value;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<size_type N, int = adl_lookup (ident<N> {})>
    static constexpr size_type value_reader (int, ident<N>) {
      return N;
    }

    template<size_type N>
    static constexpr size_type value_reader (float, ident<N>, size_type R = value_reader (0, ident<N-1> ())) {
      return R;
    }

    static constexpr size_type value_reader (float, ident<0>) {
      return 0;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<size_type Max = MAX_VALUE_READER>
    static constexpr size_type value (size_type R = value_reader (0, ident<Max> {})) {
      return R;
    }

    template<size_type N = 1, class H = meta_counter>
    static constexpr size_type next (size_type R = writer<ident<N + H::value ()>>::value) {
      return R;
    }
  };
}}



namespace atch { namespace {

  template<class Tag>
  struct meta_list {
    using   counter = atch::meta_counter<meta_list<Tag>>;
    using size_type = typename counter::size_type;

    template<size_type N, class = void>
    struct ident {
      friend auto adl_lookup (ident<N>);

      static constexpr size_type value = N;
    };

    template<class Dummy>
    struct ident<0, Dummy> {
      friend auto adl_lookup (ident<0>) {
        return atch::type_list<> {};
      }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<class Ident, class Value>
    struct writer {
      friend auto adl_lookup (ident<Ident::value>) {
        return Value {};
      }

      static constexpr size_type value = Ident::value;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<
      class State,
      class     H = meta_list,
      class Ident = typename H::template ident<H::counter::next ()>
    >
    static constexpr size_type push_state (size_type R = writer<Ident, State>::value) {
      return R;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<
      class     H = meta_list,
      size_type N = H::counter::value ()>
    using value_ident = typename H::template ident<N>;

    template<
      class     H = meta_list,
      class Ident = typename H::template value_ident<>
    >
    using value = decltype (adl_lookup (Ident {}));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    template<class... Ts, class H = meta_list>
    static constexpr bool push (
      size_type = push_state<
        typename H::template value<>::template push<Ts...>::result
      > ()
    ) { return true;}

    template<class H = meta_list>
    static constexpr void pop (
      size_type = push_state<
        typename H::template value<>::init::result
      > ()
    ) {}

    template<size_type Idx, class T, class H = meta_list>
    static constexpr void set (
      size_type = push_state<
        typename H::template value<>::template set<Idx, T>::result
      > ()
    ) {}
  };
}}

#endif /* include guard */








///////////////////////////////////////////////////////////////////////////////////////////////////////
///
///             HELPERS

namespace smart_enum
{

constexpr unsigned c_strlen( char const* str, unsigned count = 0 )
{
    return ('\0' == str[0]) ? count : c_strlen(str+1, count+1);
}

constexpr unsigned c_strcmp( char const* str1, char const* str2, unsigned count = 0 )
{
    return ('\0' == str1[count] && '\0' == str2[count]) ? 0 :
            ('\0' == str1[count] ?  -1 :
            ('\0' == str2[count] ? +1 :
                (str1[count] < str2[count] ? -1 : (str1[count] > str2[count]? 1: c_strcmp(str1, str2, count+1)))
            )
            );
}



template <int k>
struct Wrap{
  static constexpr int value = k;
};


template <typename Enum>
constexpr size_t enum_size()
{
    return Enum::MetaList::counter::value();
}

template <typename T, typename Enum>
T enum_to_int_cast(Enum enum_value)
{
    return static_cast<T>(enum_value.enum_member);
}

template <typename Enum, typename T>
constexpr Enum int_to_enum_cast(T int_value)
{
    return Enum(static_cast<typename Enum::value_t>(int_value));
}


template <int k, typename SmartEnum>
constexpr SmartEnum get()
{
    return int_to_enum_cast<SmartEnum>(SmartEnum::MetaList::template value<>:: template at<k>::result::value);
}

template <int k, typename SmartEnum>
constexpr const char* get_enum_name()
{
    return SmartEnum::MetaListNames::template value<>:: template at<k>::result::str();
}

template <int k, typename SmartEnum>
constexpr const char* get_enum_description()
{
    return SmartEnum::MetaListDescriptions::template value<>:: template at<k>::result::str();
}

//////////////////////////////////////////////////////////////////////////////
/// \brief Find smart enum
///
namespace detail
{
    template <typename SmartEnum, typename FindType, size_t CurPos, size_t Size>
    struct index_of_impl {
        static constexpr
        ssize_t find(FindType needle)
        {
            return get<CurPos, SmartEnum>() == needle
                      ? CurPos
                      : index_of_impl<SmartEnum, FindType, CurPos + 1, Size>::find(needle);
        }
    };

    template <typename SmartEnum, typename FindType, size_t Size>
    struct index_of_impl<SmartEnum, FindType, Size, Size> {
        static constexpr
        ssize_t find(FindType needle)
        {
            return -1;
        }
    };
}

template <typename SmartEnum, typename FindType>
constexpr
ssize_t index_of(FindType needle)
{
    return detail::index_of_impl<SmartEnum, FindType, 0, enum_size<SmartEnum>()>::find(needle);
}

///////////////////////////////////////////////////////////////////////////////
///  check_smart_enum

template <typename SmartEnum>
constexpr bool check(SmartEnum arg)
{
    return index_of<SmartEnum>(arg.enum_member) != -1;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief Conv Policy
///
struct NonStrictConvPolicy {
};

/////////////////////////////////////////////////////////////////////////////
///  enum_to_string

namespace detail
{
    template <typename SmartEnum, typename ConvPolicy, size_t CurPos, size_t Size>
    struct enum_to_string_impl {
        static constexpr
        const char* to_string(ssize_t pos_to_extract)
        {
            return pos_to_extract == -1
                      ? (std::is_same<ConvPolicy, NonStrictConvPolicy>::value ?  "Unknown" : (throw std::bad_alloc()), "Unknown")
                      :  (CurPos ==  pos_to_extract
                          ? get_enum_name<CurPos, SmartEnum>()
                          : enum_to_string_impl<SmartEnum, ConvPolicy, CurPos+1, Size>::to_string(pos_to_extract)
                          );
        }
    };

    template <typename SmartEnum, typename ConvPolicy, size_t Size>
    struct enum_to_string_impl<SmartEnum, ConvPolicy,  Size, Size> {
        static constexpr
        const char* to_string(ssize_t pos_to_extract)
        {
            return std::is_same<ConvPolicy, NonStrictConvPolicy>::value ?  "Unknown" : (throw std::bad_alloc()), "Unknown";
        }

    };
}

template <typename ConvPolicy = int, typename SmartEnum>
constexpr
const char* to_string(SmartEnum smart_enum)
{
    return detail::enum_to_string_impl<SmartEnum, ConvPolicy, 0, enum_size<SmartEnum>()>::to_string( index_of<SmartEnum>(smart_enum.enum_member) );
}

/////////////////////////////////////////////////////////////////////////////
///  enum_to_description

namespace detail
{
    template <typename SmartEnum, typename ConvPolicy, size_t CurPos, size_t Size>
    struct enum_to_description_impl {
        static constexpr
        const char* to_string(ssize_t pos_to_extract)
        {
            return pos_to_extract == -1
                      ? (std::is_same<ConvPolicy, NonStrictConvPolicy>::value ?  "Unknown" : (throw std::bad_alloc()), "Unknown")
                      :  (CurPos ==  pos_to_extract
                          ? get_enum_description<CurPos, SmartEnum>()
                          : enum_to_description_impl<SmartEnum, ConvPolicy, CurPos+1, Size>::to_string(pos_to_extract)
                          );
        }
    };

    template <typename SmartEnum, typename ConvPolicy, size_t Size>
    struct enum_to_description_impl<SmartEnum, ConvPolicy,  Size, Size> {
        static constexpr
        const char* to_string(ssize_t pos_to_extract)
        {
            return std::is_same<ConvPolicy, NonStrictConvPolicy>::value ?  "Unknown" : (throw std::bad_alloc()), "Unknown";
        }

    };
}

template <typename ConvPolicy = int, typename SmartEnum>
constexpr
const char* get_description(SmartEnum smart_enum)
{
    return detail::enum_to_description_impl<SmartEnum, ConvPolicy, 0, enum_size<SmartEnum>()>::to_string( index_of<SmartEnum>(smart_enum.enum_member) );
}

/////////////////////////////////////////////////////////////////////////////
///  string_to_enum
///
namespace detail {
    template <typename SmartEnum, typename ConvPolicy, size_t CurPos, size_t Size>
    struct string_to_enum_impl {
        static constexpr
        const SmartEnum to_enum(const char *str)
        {
            return c_strcmp(get_enum_name<CurPos,SmartEnum>(), str) == 0
                    ? get<CurPos, SmartEnum>()
                    : string_to_enum_impl<SmartEnum, ConvPolicy, CurPos+1, Size>::to_enum(str);
        }
    };

    template <typename SmartEnum, typename ConvPolicy, size_t Size>
    struct string_to_enum_impl<SmartEnum, ConvPolicy,  Size, Size> {
        static constexpr
        const SmartEnum to_enum(const char *str)
        {
            return (throw std::bad_alloc()), SmartEnum{};
        }
    };
}

template <typename SmartEnum, typename ConvPolicy = int>
constexpr
SmartEnum to_enum(const char *str)
{
    return detail::string_to_enum_impl<SmartEnum, ConvPolicy, 0, enum_size<SmartEnum>()>::to_enum(str);
}





//template<typename T>
//struct BaseClass_Derived2 {

//    using MetaList             = atch::meta_list  <class Derived2>;
//    using MetaListNames        = atch::meta_list  <class Derived2_names>;
//    using MetaListDescriptions = atch::meta_list  <class Derived2_descriptions>;

//    enum class BaseClass_Derived2_enum:int {
//    };
//    BaseClass_Derived2_enum enum_member;

//    constexpr BaseClass_Derived2(BaseClass_Derived2_enum member_): enum_member(member_) { }
//    constexpr BaseClass_Derived2(){ }

//    constexpr bool operator==(BaseClass_Derived2_enum en) {return en == enum_member;}
//    using is_special_enum = bool;
//    using value_t = BaseClass_Derived2_enum;
//    using underliyng_t = int;

//    constexpr operator BaseClass_Derived2_enum() const { return enum_member;}

//    using dummy_type = struct BaseClass_Derived2<T>;
//};

//struct Derived2: public BaseClass_Derived2<Derived2> {
//    using BaseClass_Derived2::BaseClass_Derived2;

////    using dummy_type::dummy_type;

//    //data
//    static constexpr BaseClass_Derived2_enum elem0 = static_cast<BaseClass_Derived2_enum>(20);
//    static constexpr int dummy_member0 = MetaList::push<Wrap<static_cast<int>(elem0)>>();
//    //names
//    struct name_helper_elem0  { constexpr static char const* str() { return "elem0"; }  };
//    static constexpr int names_dummy_member0 = MetaListNames::push<name_helper_elem0>();
//    //descriptions
//    struct descr_helper_elem0  { constexpr static char const* str() { return "descr elem0"; }  };
//    static constexpr int descr_dummy_member0 = MetaListDescriptions::push<descr_helper_elem0>();


//    //data
//    static constexpr BaseClass_Derived2_enum elem1 = static_cast<BaseClass_Derived2_enum>(21);
//    static constexpr int dummy_member1 = MetaList::push<Wrap<static_cast<int>(elem1)>>();
//    //names
//    struct name_helper_elem1  { constexpr static char const* str() { return "elem1"; }  };
//    static constexpr int names_dummy_member1 = MetaListNames::push<name_helper_elem1>();
//    //descriptions
//    struct descr_helper_elem1  { constexpr static char const* str() { return "descr elem1"; }  };
//    static constexpr int descr_dummy_member1 = MetaListDescriptions::push<descr_helper_elem1>();

//    //data
//    static constexpr BaseClass_Derived2_enum elem2 = static_cast<BaseClass_Derived2_enum>(22);
//    static constexpr int dummy_member2 = MetaList::push<Wrap<static_cast<int>(elem2)>>();
//    //names
//    struct name_helper_elem2  { constexpr static char const* str() { return "elem2"; }  };
//    static constexpr int names_dummy_member2 = MetaListNames::push<name_helper_elem2>();
//    //descriptions
//    struct descr_helper_elem2  { constexpr static char const* str() { return "descr elem2"; }  };
//    static constexpr int descr_dummy_member2 = MetaListDescriptions::push<descr_helper_elem2>();

//    //data
//    static constexpr BaseClass_Derived2_enum elem3 = static_cast<BaseClass_Derived2_enum>(23);
//    static constexpr int dummy_member3 = MetaList::push<Wrap<static_cast<int>(elem3)>>();
//    //names
//    struct name_helper_elem3  { constexpr static char const* str() { return "elem3"; }  };
//    static constexpr int names_dummy_member3 = MetaListNames::push<name_helper_elem3>();
//    //descriptions
//    struct descr_helper_elem3  { constexpr static char const* str() { return "descr elem3"; }  };
//    static constexpr int descr_dummy_member3 = MetaListDescriptions::push<descr_helper_elem3>();

//};


}


#define CONCAT_TOKENS_INTERNAL(a, b) \
    a##b

#define CONCAT_TOKENS(a,b) CONCAT_TOKENS_INTERNAL(a,b)


#define SMART_ENUM(enum_name,enum_und_type) \
    template<typename T>\
    struct BaseClass_##enum_name {\
    \
        using MetaList             = atch::meta_list  <class enum_name>;\
        using MetaListNames        = atch::meta_list  <class enum_name##_names>;\
        using MetaListDescriptions = atch::meta_list  <class enum_name##_descriptions>;\
    \
        enum class BaseClass_##enum_name##_enum:int {\
        };\
        BaseClass_##enum_name##_enum enum_member;\
    \
        constexpr BaseClass_##enum_name(BaseClass_##enum_name##_enum member_): enum_member(member_) { }\
        constexpr BaseClass_##enum_name(){ }\
    \
        constexpr bool operator==(BaseClass_##enum_name##_enum en) {return en == enum_member;}\
        using is_special_enum = bool;\
        using value_t = BaseClass_##enum_name##_enum;\
        using underliyng_t = int;\
    \
        constexpr operator BaseClass_##enum_name##_enum() const { return enum_member;}\
        using base_t = struct BaseClass_##enum_name<T>; \
    };\
\
    struct enum_name: public BaseClass_##enum_name<enum_name>

#define INITIALIZE_SMART_ENUM using base_t::base_t

#define SM_ENUM_ELEM4(elem_id, elem_value, elem_name, elem_description)\
\
static constexpr value_t elem_id = static_cast<value_t>(elem_value);\
static constexpr int CONCAT_TOKENS(dummy_member,__COUNTER__) = MetaList::push<smart_enum::Wrap<static_cast<underliyng_t>(elem_id)>>();\
\
struct name_helper_##elem_id  { constexpr static char const* str() { return elem_name; }  };\
static constexpr int CONCAT_TOKENS(names_dummy_member,__COUNTER__) = MetaListNames::push<name_helper_##elem_id>();\
\
struct descr_helper_##elem_id  { constexpr static char const* str() { return elem_description; }  };\
static constexpr int CONCAT_TOKENS(descr_dummy_member,__COUNTER__) = MetaListDescriptions::push<descr_helper_##elem_id>()

#define SM_ENUM_ELEM3_(elem_id, elem_value, elem_name)\
    SM_ENUM_ELEM4(elem_id, elem_value,elem_name,elem_name)
#define SM_ENUM_ELEM3(elem_id, elem_value, elem_name)\
    SM_ENUM_ELEM3_(elem_id, elem_value, elem_name)

#define SM_ENUM_ELEM2_(elem_id, elem_value)\
    SM_ENUM_ELEM3(elem_id, elem_value, #elem_id)
#define SM_ENUM_ELEM2(elem_id, elem_value)\
    SM_ENUM_ELEM2_(elem_id, elem_value)


#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define SM_ENUM_ELEM(...) GET_MACRO(__VA_ARGS__, SM_ENUM_ELEM4, SM_ENUM_ELEM3, SM_ENUM_ELEM2)(__VA_ARGS__)

#pragma GCC diagnostic pop




#endif // SMART_ENUM_H
