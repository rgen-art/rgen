//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_OSE_TYPE_H
#define EMP_OSE_TYPE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_ose_error.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef float real;
typedef int sint;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose { class table_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace ose { namespace rti {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class type_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef vector_map_t<astring, type_c const*> const_type_vm;
typedef vector_map_t<astring, type_c*> type_vm;

typedef vector<type_c const*> const_type_v;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name type_e
#define enum_type
#define enum_values\
    enum_value( none     )\
    enum_value( Void     )\
    enum_value( Data     )\
    enum_value( Function )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name data_e
#define enum_type
#define enum_values\
    enum_value( none    )\
    enum_value( UInt    )\
    enum_value( SInt    )\
    enum_value( Real    )\
    enum_value( eBool   )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T> EMP_NOINSTANCE(class, data_type_t) };
template <data_e t_eData> EMP_NOINSTANCE(class, data_size_t) };

#define XTMP_INSTANCIATION(x_Type, x_Data)\
template <> EMP_NOINSTANCE(class, data_type_t<x_Type>) public: static const rti::data_e value = x_Data; };\
template <> EMP_NOINSTANCE(class, data_size_t<x_Data>) public: static c_size value = sizeof(x_Type); };

// @@0 literal

XTMP_INSTANCIATION(real, rti::data_e::Real)
XTMP_INSTANCIATION(sint, rti::data_e::SInt)
XTMP_INSTANCIATION(uint, rti::data_e::UInt)
XTMP_INSTANCIATION(bool, rti::data_e::eBool)

#undef XTMP_INSTANCIATION

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class data_c;
class table_c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, type_c)
public:
    type_e m_eType = type_e::none;
    astring m_sName{};
    size_t m_stSize = 0;

    type_c(void){}
    virtual ~type_c(void);

    template <typename T>
    static EMP_RETURN bool create(T* const a_pTo, c_astring& a_sName)
    {
        EMP_RET_FALSE_IF(a_pTo == nullptr);
        a_pTo->m_eType = T::type_value;
        a_pTo->m_sName = a_sName;
        return true;
    }
};
inline type_c::~type_c(void){}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, void_c, public, type_c)
public:
    static const c_type_e type_value = type_e::Void;

    void_c(void) {}
    EMP_RETURN bool create(void) { return true; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, data_c, public, type_c)
public:
    static const c_type_e type_value = type_e::Data;

    data_e m_eData = data_e::none;

public:
    data_c(void) {}
    EMP_RETURN bool create(c_data_e a_eData)
    {
        EMP_RET_FALSE_IF(a_eData == data_e::none);
        EMP_RET_FALSE_IF(enum_to(a_eData) >= enum_count(data_e));
        m_eData = a_eData;
        switch (a_eData)
        {
#define XTMP_CASE(x_Data)\
            case rti::data_e::x_Data:\
            {\
                m_stSize = data_size_t<rti::data_e::x_Data>::value;\
                break;\
            }
            // @@0 literal
            XTMP_CASE(UInt)
            XTMP_CASE(SInt)
            XTMP_CASE(Real)
            XTMP_CASE(eBool)
#undef XTMP_CASE
            case data_e::none:
            default:
            {
                EMP_LOG_ERROR("unhandled type '#0' : #1", enum_to_astring(a_eData), enum_to(a_eData));
                return false;
            }
        }
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, function_c, public, type_c)
public:
    static const c_type_e type_value = type_e::Function;

    const_type_v m_vpReturns{};
    const_type_v m_vpParameters{};
    size_t m_stParameterSize = 0;

    function_c(void) {}
    EMP_RETURN bool add_return(type_c const* const a_pType)
    {
        EMP_RET_FALSE_IF(a_pType == nullptr);
        m_vpReturns.add(a_pType);
        return true;
    }

    EMP_RETURN bool add_parameter(type_c const* const a_pType)
    {
        EMP_RET_FALSE_IF(a_pType == nullptr);
        m_vpParameters.add(a_pType);
        m_stParameterSize += a_pType->m_stSize;
        return true;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T, typename F>
inline EMP_RETURN T* to_type_impl(F* const a_pFrom)
{
    if (a_pFrom == nullptr)
    { return nullptr; }

    if (a_pFrom->m_eType != T::type_value)
    { return nullptr; }

    return static_cast<T*>(a_pFrom);
}

template <typename T>
inline EMP_RETURN T const* to_type(type_c const* const a_pFrom)
{ return to_type_impl<T const>(a_pFrom); }

template <typename T>
inline EMP_RETURN T* to_type(type_c* const a_pFrom)
{ return to_type_impl<T>(a_pFrom); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

