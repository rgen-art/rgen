//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_TT_ENUM_H
#define EMP_TT_ENUM_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_tt_limit.h"
#include "emp_tt_literal.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_value_impl(x_Call, x_Params) x_Call x_Params
#define enum_params(...) (__VA_ARGS__)
#define enum_value(...) EMP_PP_FORWARD(EMP_PP_CAT(enum_value_, EMP_VAARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define enum_value_ex(...) EMP_PP_FORWARD(EMP_PP_CAT(enum_value_ex_, EMP_VAARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define enum_class(x_Name) EMP_PP_CAT(x_Name, num)
#define enum_type_alias(...) emp::tt::remove_reference<decltype(enum_alias<void>(emp::ast::cast<__VA_ARGS__>(0)))>
#define enum_value_alias(x_Value) enum_type_alias(decltype(x_Value))
#define enum_count(x_Type) enum_type_alias(x_Type)::count
#define enum_array_of(x_Type, x_Enum) emp::cnt::array<EMP_PP_TRY_SOLVE(x_Type), enum_count(x_Enum), true>
#define enum_array(x_Enum) enum_array_of(x_Enum, x_Enum)

#define enum_to_(x_Value) enum_value_alias(x_Value)::to_t<x_Value>()
#define enum_to__(x_Value) enum_value_alias(x_Value)::to_t<x_Value>::value
#define enum_to_type_(x_Value, x_Type) enum_value_alias(x_Value)::to<x_Value, x_Type>()
#define enum_to(x_Value) enum_value_alias(x_Value)::to(x_Value)
#define enum_to_type(x_Value, x_Type) enum_value_alias(x_Value)::to<x_Type>(x_Value)

#define enum_from_(x_Type, x_Value) enum_type_alias(x_Type)::from<x_Value>()
//#define enum_from_(x_Type, x_Value) enum_type_alias(x_Type)::from_t<x_Value>::value
#define enum_from_type_(x_Type, x_Value, x_From) enum_type_alias(x_Type)::from<x_Value, x_From>()
#define enum_from(x_Type, ...) enum_type_alias(x_Type)::from(__VA_ARGS__)

#define enum_index_to_cstr(x_Type, x_Index) enum_type_alias(x_Type)::name(x_Index)
#define enum_index_to_astring(x_Type, x_Index) emp::str::astring(enum_index_to_cstr(x_Type, x_Index))

#define enum_to_cstr(x_Value) enum_value_alias(x_Value)::name(x_Value)
#define enum_to_astring(x_Value) astring(enum_to_cstr(x_Value))

#define enum_is_valid(x_Value) enum_value_alias(x_Value)::is_valid(x_Value)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

