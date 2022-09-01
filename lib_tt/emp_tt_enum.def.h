//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_tt_enum.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef enum_name
#error enum_name NOT defined
#endif

#ifndef enum_values
#error enum_values NOT defined
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef enum_value_1
#error enum_value_1 is defined
#endif

#ifdef enum_value_2
#error enum_value_2 is defined
#endif

#ifdef enum_value_ex_2
#error enum_value_ex_2 is defined
#endif

#ifdef enum_value_ex_3
#error enum_value_ex_3 is defined
#endif

#ifndef enum_base
#define enum_base ui32
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enum class enum_name : enum_base
{
#define enum_value_1(x_Name)                     x_Name,
#define enum_value_2(x_Name, x_Value)            x_Name = x_Value,
#define enum_value_ex_2(x_Name, x_User)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_2(x_Name, x_Value)
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
};
typedef enum_name const EMP_PP_CAT(c_, enum_name);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef enum_type
template <typename Dummy = void>
EMP_NOINSTANCE(struct, enum_class(enum_name))
//{

    static constexpr c_size count =
#define enum_value_1(x_Name)                     1 +
#define enum_value_2(x_Name, x_Value)            enum_value_1(x_Name)
#define enum_value_ex_2(x_Name, x_User)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_1(x_Name)
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
        0;

    static constexpr enum_name const max = emp::tt::max_t<enum_name
#define enum_value_1(x_Name)                     ,enum_name::x_Name
#define enum_value_2(x_Name, x_Value)            enum_value_1(x_Name)
#define enum_value_ex_2(x_Name, x_User)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_1(x_Name)
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
        >::value;

public:
    static constexpr enum_name const e_values[enum_class(enum_name)::count]
    {
#define enum_value_1(x_Name)                     enum_name::x_Name,
#define enum_value_2(x_Name, x_Value)            enum_value_1(x_Name)
#define enum_value_ex_2(x_Name, x_user)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_1(x_Name)
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
    };

    static constexpr size_t const st_values[enum_class(enum_name)::count]
    {
#define enum_value_1(x_Name)                     static_cast<size_t>(enum_name::x_Name),
#define enum_value_2(x_Name, x_Value)            enum_value_1(x_Name)
#define enum_value_ex_2(x_Name, x_user)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_1(x_Name)
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
    };

    // is_valid
    //
    template <enum_name t_eValue>
    static constexpr EMP_RETURN bool is_valid_enum(void)
    { return is_valid_t<t_eValue>::value; }

    template <size_t t_stValue>
    static constexpr EMP_RETURN bool is_valid_index(void)
    { return t_stValue < to_t<max>::value; }

    static constexpr EMP_RETURN bool is_valid(enum_name a_eValue)
    { return a_eValue <= max; }

    static constexpr EMP_RETURN bool is_valid_index(c_size a_stIndex)
    { return a_stIndex <= count; }

    template <enum_name t_eValue>
    EMP_NOINSTANCE(struct, is_valid_t)
        static constexpr c_bool value = t_eValue <= max;
    };

    template <typename T>
    static EMP_RETURN enum_name from(T const& a_tFrom)
    {
        // @@0 : runtime_cast
        // @@0 : failure
        enum_name const eValue = static_cast<enum_name>(a_tFrom);
        return eValue;
    }

    template <size_t t_stValue>
    static constexpr EMP_RETURN enum_name from(void)
    {
        static_assert(is_valid_index<t_stValue>(), "");
        static_assert(is_valid_enum<static_cast<enum_name>(t_stValue)>(), "");
        return static_cast<enum_name>(t_stValue);
    }

    template <size_t t_stValue>
    EMP_NOINSTANCE(struct, from_t)
        static_assert(is_valid_index<t_stValue>(), "");
        static constexpr enum_name const value = static_cast<enum_name>(t_stValue);
    };

    // to
    //
    template <typename T = size_t>
    static EMP_RETURN T to(enum_name const a_eValue)
    {
        EMP_ASSERT(is_valid(a_eValue));
        return static_cast<T>(a_eValue);
    }

    template <enum_name t_eValue, typename T = size_t>
    static constexpr EMP_RETURN T to(void)
    {
        static_assert(is_valid_enum<t_eValue>(), "");
        static_assert(static_cast<size_t>(t_eValue) <= emp::tt::max<T>::value, "");
        static_assert(t_eValue <= e_values[count - 1], "");
        return static_cast<T>(t_eValue);
    }

    template <enum_name t_eValue, typename T = size_t>
    EMP_NOINSTANCE(struct, to_t)
        static_assert(is_valid_t<t_eValue>::value, "");
        static_assert(static_cast<size_t>(t_eValue) <= emp::tt::max<T>::value, "");
        static_assert(t_eValue <= e_values[count - 1], "");
        static constexpr c_size value = static_cast<T>(t_eValue);
    };

    // name
    //
    static EMP_RETURN emp::tt::c_aliteral name(c_size a_stIndex)
    { return name(from(a_stIndex)); }

    static EMP_RETURN emp::tt::c_aliteral name(enum_name const a_eValue)
    {
        switch (a_eValue) // @@0 c++14 array...
        {
#define enum_value_1(x_Name)                     case enum_name::x_Name: { return ALITERAL(EMP_PP_STRING(x_Name)); break; }
#define enum_value_2(x_Name, x_Value)            enum_value_1(x_Name)
#define enum_value_ex_2(x_Name, x_user)          enum_value_1(x_Name)
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_value_1(x_Name)
            enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3
            default:
            {
                EMP_ASSERT(false);
                return ALITERAL("");
            }
        };
    }
    
    // assert
    static_assert(count <= emp::tt::max<size_t>::value, "");
    static_assert(emp::tt::max<enum_base>::value <= emp::tt::max<size_t>::value, "");
    static_assert(to_t<max, size_t>::value <= emp::tt::max<size_t>::value, "");
};
template <typename D>
EMP_RETURN enum_class(enum_name)<D>const& enum_alias(enum_name const& a_rFrom);

//template <typename D>
//EMP_RETURN enum_class<D>& enum_alias_t(enum_name const& a_rFrom);

//template <typename D> constexpr emp::tt::aliteral enum_class(enum_name)<D>::names[enum_class(enum_name)<D>::count];

//template <typename D> constexpr emp::tt::aliteral enum_class(enum_name)<D>::m_sEmpty;

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef enum_name
#undef enum_values
#undef enum_base
#undef enum_type

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
