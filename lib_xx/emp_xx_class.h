//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_CLASS_H
#define EMP_XX_CLASS_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_xx_pragma.h"

#include "emp_hh_stddef.h"
#include "emp_hh_tuple.h"

#include "emp_pp_forward.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx  { template <typename> class type_t; } }

#define EMP_RFX_FRIEND(...)\
    friend class emp::rfx::type_t<EMP_PP_TRY_SOLVE(__VA_ARGS__)>;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TYPEDEF_IMP(x_Type, x_Alias)\
    typedef x_Type const EMP_PP_CAT(c_, x_Alias);\
    typedef x_Type* const EMP_PP_CAT(pc_, x_Alias);\
    typedef x_Type const* const EMP_PP_CAT(cpc_, x_Alias);\

#define EMP_TYPEDEF_EX(x_Type, x_Alias)\
    typedef x_Type x_Alias;\
    EMP_TYPEDEF_IMP(x_Type, x_Alias)

#define EMP_TYPEDEF(x_Type)\
    EMP_TYPEDEF_IMP(x_Type, x_Type)

#define EMP_FORWARD_enum_2(x_Mod, x_Name)\
    enum class x_Name : x_Mod;\
    EMP_TYPEDEF(x_Name)

#define EMP_FORWARD_TYPE(x_Class, x_Mod, x_Name)\
    x_Mod x_Class x_Name;\
    EMP_TYPEDEF_EX(x_Mod x_Class x_Name, x_Name)

#define EMP_FORWARD_class_1(x_Name) EMP_FORWARD_TYPE(class, EMP_PP_EMPTY(), x_Name)
#define EMP_FORWARD_class_2(x_Mod, x_Name) EMP_FORWARD_TYPE(class, x_Mod, x_Name)

#define EMP_FORWARD_struct_1(x_Name) EMP_FORWARD_TYPE(struct, EMP_PP_EMPTY(), x_Name)
#define EMP_FORWARD_struct_2(x_Mod, x_Name) EMP_FORWARD_TYPE(struct, x_Mod, x_Name)

#define EMP_FORWARD(x_Class, ...) EMP_PP_FORWARD(EMP_PP_CAT(EMP_FORWARD_, EMP_PP_CAT(x_Class, EMP_PP_CAT(_, EMP_VAARGS_COUNT(__VA_ARGS__))))(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_DEFCOPY(...)\
public:\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)(void) = default;\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)(EMP_PP_TRY_SOLVE(__VA_ARGS__) const& a_rFrom) = default;\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)& operator=(EMP_PP_TRY_SOLVE(__VA_ARGS__) const& a_rFrom) = default;\
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_NOCOPY(...)\
public:\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)(EMP_PP_TRY_SOLVE(__VA_ARGS__) const& a_rFrom) = delete;\
    EMP_PP_TRY_SOLVE(__VA_ARGS__) const& operator=(EMP_PP_TRY_SOLVE(__VA_ARGS__) const& a_rFrom) = delete;\
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_NOINSTANCE(...)\
public:\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)(void) = delete;\
    virtual ~EMP_PP_TRY_SOLVE(__VA_ARGS__)(void) = delete;\
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_NODEFAULT(...)\
public:\
    EMP_PP_TRY_SOLVE(__VA_ARGS__)(void) = delete;\
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XX_TYPE_ACCESS_struct public:
#define EMP_XX_TYPE_ACCESS_class private:
#define EMP_XX_TYPE_ACCESS(x_Type) EMP_PP_CAT(EMP_XX_TYPE_ACCESS_, x_Type)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_(x_Type, ...)\
x_Type EMP_PP_FORWARD(EMP_PP_TRY_SOLVE(__VA_ARGS__))\
{\
EMP_PP_FORWARD(EMP_RFX_FRIEND(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_STRUCT(...)\
struct EMP_PP_TRY_SOLVE(__VA_ARGS__);\
EMP_TYPEDEF(EMP_PP_TRY_SOLVE(__VA_ARGS__))\
struct EMP_PP_FORWARD(EMP_PP_TRY_SOLVE(__VA_ARGS__))\
{\
EMP_PP_FORWARD(EMP_RFX_FRIEND(__VA_ARGS__))\
EMP_PP_FORWARD(EMP_XX_DEFCOPY(__VA_ARGS__))\
public:


#define EMP_CLASS(...)\
class EMP_PP_TRY_SOLVE(__VA_ARGS__);\
EMP_TYPEDEF(EMP_PP_TRY_SOLVE(__VA_ARGS__))\
class EMP_PP_FORWARD(EMP_PP_TRY_SOLVE(__VA_ARGS__))\
{\
EMP_PP_FORWARD(EMP_RFX_FRIEND(__VA_ARGS__))\
EMP_PP_FORWARD(EMP_XX_NOCOPY(__VA_ARGS__))\
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_DEFCOPY(x_Type, ...)\
    EMP_PP_FORWARD(EMP_(x_Type, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_XX_DEFCOPY(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_NOCOPY(x_Type, ...)\
    EMP_PP_FORWARD(EMP_(x_Type, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_XX_NOCOPY(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_NOINSTANCE(x_Type, ...)\
    EMP_PP_FORWARD(EMP_NOCOPY(x_Type, __VA_ARGS__))\
    EMP_PP_FORWARD(EMP_XX_NOINSTANCE(__VA_ARGS__))\
    EMP_XX_TYPE_ACCESS(x_Type)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_TYPEDEF_BASE_2(x_Modifier1, ...)\
    typedef EMP_PP_TRY_SOLVE(__VA_ARGS__) tbase1;\
    typedef std::tuple<EMP_PP_TRY_SOLVE(__VA_ARGS__)> tbases;

#define EMP_TYPEDEF_BASE_4(x_Modifier1, x_Base1, x_Modifier2, ...)\
    typedef EMP_PP_TRY_SOLVE(x_Base1) tbase1;\
    typedef EMP_PP_TRY_SOLVE(__VA_ARGS__) tbase2;\
    typedef std::tuple<x_Base1, EMP_PP_TRY_SOLVE(__VA_ARGS__)> tbases;

#define EMP_TYPEDEF_BASE_6(x_Modifier1, x_Base1, x_Modifier2, x_Base2, x_Modifier3, ...)\
    typedef EMP_PP_TRY_SOLVE(x_Base1) tbase1;\
    typedef EMP_PP_TRY_SOLVE(x_Base2) tbase2;\
    typedef EMP_PP_TRY_SOLVE(__VA_ARGS__) tbase3;\
    typedef std::tuple<x_Base1, x_Base2, EMP_PP_TRY_SOLVE(__VA_ARGS__)> tbases;

#define EMP_TYPEDEF_BASE(...)\
    EMP_PP_FORWARD(EMP_PP_CAT(EMP_TYPEDEF_BASE_, EMP_VAARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_INHERITANCE_2(x_Modifier1, ...)\
    x_Modifier1 EMP_PP_TRY_SOLVE(__VA_ARGS__)

#define EMP_INHERITANCE_4(x_Modifier1, x_Base1, x_Modifier2, ...)\
    x_Modifier1 EMP_PP_TRY_SOLVE(x_Base1), x_Modifier2 EMP_PP_TRY_SOLVE(__VA_ARGS__)

#define EMP_INHERITANCE_6(x_Modifier1, x_Base1, x_Modifier2, x_Base2, x_Modifier3, ...)\
    x_Modifier1 EMP_PP_TRY_SOLVE(x_Base1), x_Modifier2 EMP_PP_TRY_SOLVE(x_Base2), x_Modifier3 EMP_PP_TRY_SOLVE(__VA_ARGS__)

#define EMP_INHERITANCE(...)\
    EMP_PP_EXPAND(EMP_PP_CAT(EMP_INHERITANCE_, EMP_VAARGS_COUNT(__VA_ARGS__))(__VA_ARGS__))
//    EMP_PP_FUCK(EMP_PP_FUCK(EMP_PP_CAT(EMP_INHERITANCE_, EMP_PP_EXPAND(EMP_VAARGS_COUNT(__VA_ARGS__))))(__VA_ARGS__))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_BASE(x_Type, x_Name, ...)\
x_Type EMP_PP_TRY_SOLVE(x_Name) : EMP_PP_FORWARD(EMP_INHERITANCE(__VA_ARGS__))\
{\
    EMP_RFX_FRIEND(x_Name)\
public:\
    EMP_PP_FORWARD(EMP_TYPEDEF_BASE(__VA_ARGS__))\
    EMP_XX_TYPE_ACCESS(x_Type)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_NOCOPY_BASE(x_Type, x_Name, ...)\
    EMP_PP_FORWARD(EMP_BASE(x_Type, x_Name, __VA_ARGS__))\
    EMP_XX_NOCOPY(x_Name)\
    EMP_XX_TYPE_ACCESS(x_Type)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_NOINSTANCE_BASE(x_Type, x_Name, ...)\
    EMP_PP_FORWARD(EMP_NOCOPY_BASE(x_Type, x_Name, __VA_ARGS__))\
    EMP_XX_NOINSTANCE(x_Name)\
    EMP_XX_TYPE_ACCESS(x_Type)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_FIELD_1(x_Type, x_Name)\
    x_Type x_Name;\
    auto& x_Name##_(x_Type const& a_Value) { x_Name = a_Value; return *this; }

#define EMP_FIELD_0(x_Type, x_Name, ...)\
    x_Type x_Name = __VA_ARGS__;\
    auto& x_Name##_(x_Type const& a_Value) { x_Name = a_Value; return *this; }

#define EMP_FIELD(x_Type, ...)\
    EMP_PP_CAT(EMP_FIELD_, EMP_PP_EQUAL(1, EMP_PP_FORWARD(EMP_VAARGS_COUNT(__VA_ARGS__)))(x_Type, __VA_ARGS__))

#define EMP_FIELD_MAX(x_Type, x_Name) EMP_FIELD(x_Type, x_Name, EMP_TT_MAX(x_Name))


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

