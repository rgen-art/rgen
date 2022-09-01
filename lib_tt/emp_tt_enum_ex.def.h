//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
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
 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_value_1(x_Name)                     enum_forward(x_Name))
#define enum_value_2(x_Name, x_Value)            enum_forward(x_Name, x_Value))
#define enum_value_ex_2(x_Name, x_User)          enum_forward(x_Name, EMP_PP_TRY_SOLVE(x_User))
#define enum_value_ex_3(x_Name, x_Value, x_User) enum_forward(x_Name, x_Value, EMP_PP_TRY_SOLVE(x_User))
#define enum_forward(...) EMP_PP_FORWARD(enum_value_ex_impl(__VA_ARGS__))
        enum_values
#undef enum_value_1
#undef enum_value_2
#undef enum_value_ex_2
#undef enum_value_ex_3

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#undef enum_values
#undef enum_value_ex_impl

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
